#include "ienium/glow/core/Vector.hpp"
#include "ienium/utils/logger/ieniumlogger.hpp"
#include <cmath>
#include <string>

namespace ienium
{
    void Vector2::Normalize ()
    {
        double length = std::sqrt(x*x + y*y);
        x = x/length;
        y = y/length;
    }

    Vector2 Vector2::Normalized () const
    {
        double length = std::sqrt(x*x + y*y);
        return Vector2 (x/length, y/length);
    }

    double Vector2::Length () const
    {
        return std::sqrt(x*x + y*y);
    }

    double Vector2::SqrLength () const
    {
        return x*x + y*y;
    }

    double Vector2::Dot (const Vector2& other) const
    {
        return x * other.x + y * other.y;
    }

    void Vector2::Add (const Vector2& other)
    {
        x += other.x;
        y += other.y;
    }
    void Vector2::Sub (const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
    }

    void Vector2::Add (const double& scalar)
    {
        x += scalar;
        y += scalar;
    }
    void Vector2::Sub (const double& scalar)
    {
        x -= scalar;
        y -= scalar;
    }
    void Vector2::Mul (const double& scalar)
    {
        x *= scalar;
        y *= scalar;
    }
    void Vector2::Div (const double& scalar)
    {
        if (scalar == 0)
            utils::LoggerManager::GetInstance().GetLogger()->Log (utils::IENIUM_ERROR, "Tried to divide Vector2 by 0");
        x /= scalar;
        y /= scalar;
    }

    Vector2 Vector2::operator+ (const Vector2& other) const
    {
        double _x = x + other.x;
        double _y = y + other.y;
        return Vector2 (_x, _y);
    }

    Vector2 Vector2::operator- (const Vector2& other) const
    {
        double _x = x - other.x;
        double _y = y - other.y;
        return Vector2 (_x, _y);
    }

    double Vector2::operator* (const Vector2& other) const
    {
        return this->Dot (other);
    }

    Vector2 Vector2::operator+ (const double& scalar) const
    {
        double _x = x + scalar;
        double _y = y + scalar;
        return Vector2 (_x, _y);
    }

    Vector2 Vector2::operator- (const double& scalar) const
    {
        double _x = x - scalar;
        double _y = y - scalar;
        return Vector2 (_x, _y);
    }

    Vector2 Vector2::operator* (const double& scalar) const
    {
        double _x = x * scalar;
        double _y = y * scalar;
        return Vector2 (_x, _y);
    }

    Vector2 Vector2::operator/ (const double& scalar) const
    {
        double _x = x / scalar;
        double _y = y / scalar;
        return Vector2 (_x, _y);
    }

    Vector2 operator+ (double& scalar, const Vector2& v) 
    {
        double _x =  scalar + v.x;
        double _y =  scalar + v.y;
        return Vector2 (_x, _y);
    }

    Vector2 operator- (double& scalar, const Vector2& v)
    {
        double _x =  scalar - v.x;
        double _y =  scalar - v.y;
        return Vector2 (_x, _y);
    }

    Vector2 operator* (double& scalar, const Vector2& v)
    {
        double _x =  scalar * v.x;
        double _y =  scalar * v.y;
        return Vector2 (_x, _y);
    }

    std::string Vector2::ToString () const
    {
        return std::string ("( " + std::to_string(x) + " , " + std::to_string (y) + " )");
    }

    std::ostream& operator<< (std::ostream& os, const Vector2& v)
    {
        return os << v.ToString ();
    }
}