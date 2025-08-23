#include <cassert>
#include <cmath>
#include <print>

#include "ienium/glow/core/Vector.hpp"

using namespace ienium;

// --- Helper macro for testing ---
#define ASSERT_EQ(actual, expected) \
    do { \
        auto act = (actual); \
        auto exp = (expected); \
        if (!(std::fabs(act - exp) < 1e-9)) { \
            std::println("❌ Test failed: {} != {}", act, exp); \
            assert(false); \
        } \
    } while (0)

#define ASSERT_TRUE(cond, msg) \
    do { \
        if (!(cond)) { \
            std::println("❌ Test failed: {}", msg); \
            assert(false); \
        } \
    } while (0)

int main() {
    Vector2 a(3.0, 4.0);
    Vector2 b(1.0, 2.0);

    // --- Length and SqrLength ---
    ASSERT_EQ(a.Length(), 5.0);
    ASSERT_EQ(b.SqrLength(), 5.0);

    // --- Addition and subtraction operators ---
    Vector2 c = a + b;
    ASSERT_EQ(c.X(), 4.0);
    ASSERT_EQ(c.Y(), 6.0);

    Vector2 d = a - b;
    ASSERT_EQ(d.X(), 2.0);
    ASSERT_EQ(d.Y(), 2.0);

    // --- Dot product ---
    ASSERT_EQ(a.Dot(b), 11.0);
    ASSERT_EQ(a * b, 11.0); // operator* also dot

    // --- Scalar operators ---
    Vector2 e = a * 2.0;
    ASSERT_EQ(e.X(), 6.0);
    ASSERT_EQ(e.Y(), 8.0);

    Vector2 f = b / 2.0;
    ASSERT_EQ(f.X(), 0.5);
    ASSERT_EQ(f.Y(), 1.0);

    // --- Normalization ---
    Vector2 normA = a.Normalized();
    ASSERT_EQ(normA.Length(), 1.0);

    a.Normalize();
    ASSERT_EQ(a.Length(), 1.0);

    // --- Friend operators with scalar on left side ---
    double s = 5.0;
    Vector2 g = s + b;
    ASSERT_EQ(g.X(), 6.0);
    ASSERT_EQ(g.Y(), 7.0);

    Vector2 h = s - b;
    ASSERT_EQ(h.X(), 4.0);
    ASSERT_EQ(h.Y(), 3.0);

    Vector2 i = s * b;
    ASSERT_EQ(i.X(), 5.0);
    ASSERT_EQ(i.Y(), 10.0);

    // --- In-place Add / Sub (vector overload) ---
    Vector2 j(2.0, 3.0);
    j.Add(Vector2(1.0, 1.0)); // j = (3, 4)
    ASSERT_EQ(j.X(), 3.0);
    ASSERT_EQ(j.Y(), 4.0);

    j.Sub(Vector2(2.0, 2.0)); // j = (1, 2)
    ASSERT_EQ(j.X(), 1.0);
    ASSERT_EQ(j.Y(), 2.0);

    // --- In-place Add / Sub (scalar overload) ---
    Vector2 k(1.0, 1.0);
    k.Add(5.0); // k = (6, 6)
    ASSERT_EQ(k.X(), 6.0);
    ASSERT_EQ(k.Y(), 6.0);

    k.Sub(2.0); // k = (4, 4)
    ASSERT_EQ(k.X(), 4.0);
    ASSERT_EQ(k.Y(), 4.0);

    // --- In-place Mul / Div ---
    Vector2 m(2.0, 3.0);
    m.Mul(2.0); // m = (4, 6)
    ASSERT_EQ(m.X(), 4.0);
    ASSERT_EQ(m.Y(), 6.0);

    m.Div(2.0); // m = (2, 3)
    ASSERT_EQ(m.X(), 2.0);
    ASSERT_EQ(m.Y(), 3.0);

    std::println("✅ All Vector2 unit tests passed!");
}