#pragma once


#include <format>
#include <ostream>
#include <string>

namespace ienium
{
    class Vector2
    {
        public:
        Vector2 (double _x = 0.0, double _y = 0.0) : x(_x), y(_y) {}


        inline double X () const { return x; }
        inline double Y () const { return y; }
        inline void X (double _x) { x = _x; }
        inline void Y (double _y) { y = _y; }

        void Normalize ();
        Vector2 Normalized () const;
        double Length () const;
        double SqrLength () const;

        double Dot (const Vector2& other) const;

        void Add (const Vector2& other);
        void Sub (const Vector2& other);

        void Add (const double& scalar);
        void Sub (const double& scalar);
        void Mul (const double& scalar);
        void Div (const double& scalar);


        Vector2 operator+ (const Vector2& other) const;
        Vector2 operator- (const Vector2& other) const;
        double operator* (const Vector2& other) const;

        Vector2 operator+ (const double& scalar) const;
        Vector2 operator- (const double& scalar) const;
        Vector2 operator* (const double& scalar) const;
        Vector2 operator/ (const double& scalar) const;

        friend Vector2 operator+ (double& scalar, const Vector2& v);
        friend Vector2 operator- (double& scalar, const Vector2& v);
        friend Vector2 operator* (double& scalar, const Vector2& v);
        
        // String output
        inline std::string ToString () const;
        friend std::ostream& operator<< (std::ostream& os, const Vector2& v);


        private:
        double x;
        double y;
    };
}

namespace std
{
    template<>
    struct formatter<ienium::Vector2> : std::formatter<std::string>
    {
        auto format (const ienium::Vector2& v, format_context& ctx) const
        {
            return std::formatter<std::string>::format (
                v.ToString(), ctx
            );
        }
    };
}