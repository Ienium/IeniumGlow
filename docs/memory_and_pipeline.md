# Memory and Pipeline Documentation

## High-Level Pipeline Flow

```mermaid
%%{init: {'theme':'base', 'themeVariables': { 'primaryColor':'#2c2f40','primaryTextColor':'#fff','primaryBorderColor':'#000','lineColor':'#fff'}}}%%
flowchart LR
	subgraph Legend
        P[Implemented]
        Q[Not Yet Implemented]
    end 
        
    subgraph Graph[" "]
        A[Frame Start]
        B[Reset Memory Chunks]
        C[Free Unused Memory Blocks]
        D[Expand Memory Blocks]
        E[Reset All Batches]
        
        

        
        F[Draw Calls]
        
        G[Sort Batches]
        H[Thread 1: Process Batch A]
        I[Thread 2: Process Batch B]
        J[Thread 3: Process Batch C]
        K[Thread 1: Batch A Ready]
        L[Thread 2: Batch B Ready]
        M[Thread 3: Batch C Ready]
        N[Send Batch to GPU and Draw]

        A --> B --> C --> D --> E --> F --> G
        G --> H & I & J
        H --> K
        I --> L
        J --> M
        K & L & M --> N
    end
	
	
   
  
    style Q fill:#ffb366,stroke:#000,stroke-width:2px, color:#000
    style G fill:#ffb366,stroke:#000,stroke-width:2px, color:#000

    style Legend fill:None, stroke:None,color:#ffffff
    style Graph fill:None, stroke:None,color:#ffffff 
    

```

## Batch Processing Pipeline
```mermaid
%%{init: {'theme':'base', 'themeVariables': { 'primaryColor':'#2c2f40','primaryTextColor':'#fff','primaryBorderColor':'#000','lineColor':'#fff'}}}%%
    flowchart TB
        A[Request Memory]
        B[Render Command]
        C[Classify Render Command]
        D[Store Render Command in allocated Memory]
        E[Extract Render Command from memory]
        F[Owerwrite Memory with Vertex Data]
        G[Request Memory for EBO]
        H[Construct EBO]
        I[Send Memory to GPU]

        A --> B --> C --> D --> E --> F --> G --> H --> I

```

## Memory Chunk Layout
<img src="resources/memory_chunk_layout.png" alt="Alt text" width=550>

## Memory Block Layout
<img src="resources/memory_block_layout.png" alt="Alt text" width=178>

## Memory Manager Pipeline (Render Usecase)

```mermaid
  flowchart TD
    Start([Frame Start]) --> Reset[Reset All Memory Chunks]
    Reset --> CheckExpanded{Expanded Memory<br/>Block Available?}
    
    CheckExpanded -->|Yes| UseExpanded[Use Expanded Memory Block<br/>Free Old Memory Block]
    CheckExpanded -->|No| CheckNeedExpand
    UseExpanded --> CheckNeedExpand
    
    CheckNeedExpand{Need to<br/>Expand Block?}
    CheckNeedExpand -->|Yes| ParallelAlloc[Allocate Expanded<br/>Memory Block<br/>Parallel Process]
    CheckNeedExpand -->|No| MainProcess
    
    ParallelAlloc -->|Parallel| MemoryAllocated(Expanded Memory Block Allocated)
    
    MainProcess[Memory Request Comes In]
    MainProcess --> CheckMain{Required Memory<br/>≤ Available Memory<br/>in Main Block?}
    
    CheckMain -->|Yes| AllocMain[Allocate from<br/>Main Memory Block]
    CheckMain -->|No| CheckBackup{Required Memory<br/>≤ Available Memory<br/>in Backup Block?}
    
    CheckBackup -->|Yes| AllocBackup[Allocate from<br/>Backup Memory Block]
    CheckBackup -->|No| AllocNew[Allocate new Backup Block]
    
    AllocNew --> UseNew[Allocate from new Backup Block]
    
    AllocMain --> MainProcess
    AllocBackup --> MainProcess
    UseNew --> MainProcess
    MainProcess --> |Frame Start| Start
```
## Thread Execution Timeline (current Target)

```mermaid
gantt
    title Pipelined Batch Rendering Timeline
    dateFormat x
    axisFormat %L
    
  section Thread 1
    Prepare Batch A    :active, t1a, 0, 50
    Prepare Batch D    :active, t1e, 55, 115
    Prepare Batch G    :active, t1i, 120, 160
    
    section Thread 2
    Prepare Batch B    :active, t2b, 5, 65
    Prepare Batch E    :active, t2f, 70, 135
    Prepare Batch H    :active, t2j, 140, 190
    
    section Thread 3
    Prepare Batch C    :active, t3c, 10, 60
    Prepare Batch F    :active, t3g, 65, 175
    Prepare Batch I    :active, t3k, 180, 215
    
    section Main Thread
    Submit A          :crit, ma, 55, 65
    Submit B          :crit, mb, 70, 85
    Submit C          :crit, mc, 90, 100
    Submit D          :crit, md, 120, 140
    Submit E          :crit, me, 145, 165
    Submit F          :crit, mf, 180, 205
    Submit G          :crit, mg, 210, 220
    Submit H          :crit, mh, 225, 235
    Submit I          :crit, mi, 240, 250
```

This workflow is currently not possible. The Main-Thread waits until all Batches are processed.