# Pipelined Rendering with Memory Reuse

## High-Level Pipeline Flow

```mermaid
flowchart TB
    A[Frame Start] --> B[Reset Memory Pools]
    B --> C[Submit Sprites to Batcher]
    C --> D[Sort & Group Sprites]
    D --> E[Create Batches]
    
    E --> F1[Thread 1: Prepare Batch A]
    E --> F2[Thread 2: Prepare Batch B]
    E --> F3[Thread 3: Prepare Batch C]
    E --> F4[Thread 4: Prepare Batch D]
    
    F1 --> G1[Batch A Ready]
    F2 --> G2[Batch B Ready]
    F3 --> G3[Batch C Ready]
    F4 --> G4[Batch D Ready]
    
    G1 --> H[Main Thread: Submit in Order]
    G2 --> H
    G3 --> H
    G4 --> H
    
    H --> I[glBufferSubData]
    I --> J[ReleaseChunk]
    J --> K[Memory Available for Reuse]
    K --> L[Draw Call]

```

## Memory Lifecycle State Machine

```mermaid
stateDiagram-v2
    [*] --> Free
    Free --> Allocated : RequestMemoryChunk()
    Allocated --> FillingData : Fill vertex/index data
    FillingData --> ReadyForSubmit : Batch preparation complete
    ReadyForSubmit --> GPUCopy : glBufferSubData()
    GPUCopy --> Free : ReleaseChunk()
    Free --> Allocated : Reuse for next batch
```

## Thread Execution Timeline

```mermaid
gantt
    title Pipelined Batch Rendering Timeline
    dateFormat X
    axisFormat %Lms
    
    section Thread 1
    Prepare Batch A    :active, t1a, 0, 5
    Prepare Batch E    :active, t1e, 10, 15
    Prepare Batch I    :active, t1i, 20, 25
    
    section Thread 2
    Prepare Batch B    :active, t2b, 2, 7
    Prepare Batch F    :active, t2f, 12, 17
    Prepare Batch J    :active, t2j, 22, 27
    
    section Thread 3
    Prepare Batch C    :active, t3c, 4, 9
    Prepare Batch G    :active, t3g, 14, 19
    Prepare Batch K    :active, t3k, 24, 29
    
    section Thread 4
    Prepare Batch D    :active, t4d, 6, 11
    Prepare Batch H    :active, t4h, 16, 21
    Prepare Batch L    :active, t4l, 26, 31
    
    section Main Thread
    Submit A          :crit, ma, 5, 6
    Submit B          :crit, mb, 7, 8
    Submit C          :crit, mc, 9, 10
    Submit D          :crit, md, 11, 12
    Submit E          :crit, me, 15, 16
    Submit F          :crit, mf, 17, 18
    
    section GPU
    Process A-D       :gpu1, 5, 12
    Process E-H       :gpu2, 15, 22
    Process I-L       :gpu3, 25, 32
```

## Memory Pool Reuse Pattern

```mermaid
flowchart LR
    subgraph "Memory Pool (4 chunks)"
        M1[Chunk 1]
        M2[Chunk 2] 
        M3[Chunk 3]
        M4[Chunk 4]
    end
    
    subgraph "Time: 0-10ms"
        A[Batch A] --> M1
        B[Batch B] --> M2
        C[Batch C] --> M3
        D[Batch D] --> M4
    end
    
    subgraph "Time: 5ms (A submitted)"
        M1 --> E[Batch E]
    end
    
    subgraph "Time: 7ms (B submitted)"
        M2 --> F[Batch F]
    end
    
    subgraph "Time: 9ms (C submitted)"
        M3 --> G[Batch G]
    end
    
    subgraph "Time: 11ms (D submitted)"
        M4 --> H[Batch H]
    end
    
    style M1 fill:#ff9999
    style M2 fill:#99ff99
    style M3 fill:#9999ff
    style M4 fill:#ffff99
```

## Batch Processing States

```mermaid
flowchart TD
    A[Sprite Submitted] --> B{Sort by Texture}
    B --> C[Group into Batches]
    C --> D{Batch Size Check}
    D -->|\< Max Size| E[Add to Current Batch]
    D -->|\> Max Size| F[Start New Batch]
    E --> G[Batch Ready Queue]
    F --> G
    
    G --> H[Worker Thread Available?]
    H -->|Yes| I[Assign to Thread]
    H -->|No| J[Wait in Queue]
    J --> H
    
    I --> K[RequestMemoryChunk]
    K --> L{Memory Available?}
    L -->|Yes| M[Fill Vertex Data]
    L -->|No| N[Error Handling]
    N --> M

    
    M --> O[Mark as Ready]
    O --> P[Main Thread Submits]
    P --> Q[glBufferSubData]
    Q --> R[ReleaseChunk]
    R --> S[Draw Call]
    
    S --> Z{Chunks marked for creation?}
    Z -->|Yes| ZA[Create new chunks]
    ZA--> ZB[EndFrame]
    Z -->|No| ZB
```

## Synchronization Points

```mermaid
sequenceDiagram
    participant App as Application
    participant Batcher as Batch System
    participant Thread1 as Worker Thread 1
    participant Thread2 as Worker Thread 2
    participant Main as Main Thread
    participant GPU as GPU
    
    App->>Batcher: Submit 1000 sprites
    Batcher->>Batcher: Sort & group into 4 batches
    
    par Parallel Preparation
        Batcher->>Thread1: Prepare Batch A
        Batcher->>Thread2: Prepare Batch B
    and
        Note over Thread1: RequestMemoryChunk(16KB)
        Thread1->>Thread1: Fill vertex data
        Note over Thread2: RequestMemoryChunk(12KB)
        Thread2->>Thread2: Fill vertex data
    end
    
    Thread1->>Main: Batch A Ready
    Main->>GPU: glBufferSubData(Batch A)
    Main->>Batcher: ReleaseChunk(A)
    Main->>GPU: DrawElements(Batch A)
    
    Thread2->>Main: Batch B Ready
    Main->>GPU: glBufferSubData(Batch B)
    Main->>Batcher: ReleaseChunk(B)
    Main->>GPU: DrawElements(Batch B)
    
    Note over GPU: Processes batches in parallel with CPU
```

## Error Handling Flow

```mermaid
flowchart TD
    A[RequestMemoryChunk] --> B{Chunk Available?}
    B -->|Yes| C[Success Path]
    B -->|No| D[Memory Pool Full]
    
    D --> E{Try Larger Pool?}
    E -->|Available| F[Use Larger Chunk]
    E -->|None Available| G[Create new Chunk]
    
    G --> H[Use new Chunk]
    H -->J
    
    F --> J[Mark for creation of new chunks]
    J --> C
    
    
    C --> K[Fill Data & Submit]
```