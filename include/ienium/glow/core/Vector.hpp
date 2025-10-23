#pragma once

#include <format>
#include <ostream>
#include <string>

namespace ienium
{
    class Vector2
    {
      public:
        Vector2() : x(0), y(0) {}
        Vector2(double this_x, double this_y) : x(this_x), y(this_y) {}
        Vector2(double equal_length) : x(equal_length), y(equal_length) {}

        [[nodiscard]] inline double X() const { return x; }
        [[nodiscard]] inline double Y() const { return y; }
        inline void                 X(double new_x) { x = new_x; }
        inline void                 Y(double new_y) { y = new_y; }

        void                  Normalize();
        [[nodiscard]] Vector2 Normalized() const;
        [[nodiscard]] double  Length() const;
        [[nodiscard]] double  SqrLength() const;

        [[nodiscard]] double Dot(const Vector2& other) const;

        void Add(const Vector2& other);
        void Sub(const Vector2& other);

        void Add(const double& scalar);
        void Sub(const double& scalar);
        void Mul(const double& scalar);
        void Div(const double& scalar);

        Vector2 operator+(const Vector2& other) const;
        Vector2 operator-(const Vector2& other) const;
        double  operator*(const Vector2& other) const;

        Vector2 operator+(const double& scalar) const;
        Vector2 operator-(const double& scalar) const;
        Vector2 operator*(const double& scalar) const;
        Vector2 operator/(const double& scalar) const;

        friend Vector2 operator+(double& scalar, const Vector2& vec);
        friend Vector2 operator-(double& scalar, const Vector2& vec);
        friend Vector2 operator*(double& scalar, const Vector2& vec);

        // String output
        [[nodiscard]] inline std::string ToString() const;
        friend std::ostream&             operator<<(std::ostream& out_stream, const Vector2& vec);

        // Standard Vectors
        static Vector2 Zero() { return {0, 0}; }
        static Vector2 One() { return {1, 1}; }

        static Vector2 Up() { return {0, 1}; }
        static Vector2 Down() { return {0, -1}; }
        static Vector2 Right() { return {1, 0}; }
        static Vector2 Left() { return {-1, 0}; }

      private:
        double x;
        double y;
    };
} // namespace ienium

namespace std
{
    template <>
    struct formatter<ienium::Vector2> : std::formatter<std::string>
    {
        auto format(const ienium::Vector2& vec, format_context& ctx) const { return std::formatter<std::string>::format(vec.ToString(), ctx); }
    };
} // namespace std