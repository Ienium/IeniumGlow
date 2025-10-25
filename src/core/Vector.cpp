#include "ienium/glow/core/Vector.hpp"

#include <cmath>
#include <string>

#include "ienium/utils/logger/ieniumlogger.hpp"

namespace ienium
{
    void Vector2::Normalize()
    {
        double length = std::sqrt((x * x) + (y * y));
        x             = x / length;
        y             = y / length;
    }

    Vector2 Vector2::Normalized() const
    {
        double length = std::sqrt((x * x) + (y * y));
        return {x / length, y / length};
    }

    double Vector2::Length() const
    {
        return std::sqrt((x * x) + (y * y));
    }

    double Vector2::SqrLength() const
    {
        return (x * x) + (y * y);
    }

    double Vector2::Dot(const Vector2& other) const
    {
        return (x * other.x) + (y * other.y);
    }

    void Vector2::Add(const Vector2& other)
    {
        x += other.x;
        y += other.y;
    }
    void Vector2::Sub(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
    }

    void Vector2::Add(const double& scalar)
    {
        x += scalar;
        y += scalar;
    }
    void Vector2::Sub(const double& scalar)
    {
        x -= scalar;
        y -= scalar;
    }
    void Vector2::Mul(const double& scalar)
    {
        x *= scalar;
        y *= scalar;
    }
    void Vector2::Div(const double& scalar)
    {
        if (scalar == 0)
            utils::LoggerManager::GetInstance().GetLogger()->Log(utils::IENIUM_ERROR, "Tried to divide Vector2 by 0");
        x /= scalar;
        y /= scalar;
    }

    Vector2 Vector2::operator+(const Vector2& other) const
    {
        double ret_x = x + other.x;
        double ret_y = y + other.y;
        return {ret_x, ret_y};
    }

    Vector2 Vector2::operator-(const Vector2& other) const
    {
        double ret_x = x - other.x;
        double ret_y = y - other.y;
        return {ret_x, ret_y};
    }

    double Vector2::operator*(const Vector2& other) const
    {
        return this->Dot(other);
    }

    Vector2 Vector2::operator+(const double& scalar) const
    {
        double ret_x = x + scalar;
        double ret_y = y + scalar;
        return {ret_x, ret_y};
    }

    Vector2 Vector2::operator-(const double& scalar) const
    {
        double ret_x = x - scalar;
        double ret_y = y - scalar;
        return {ret_x, ret_y};
    }

    Vector2 Vector2::operator*(const double& scalar) const
    {
        double ret_x = x * scalar;
        double ret_y = y * scalar;
        return {ret_x, ret_y};
    }

    Vector2 Vector2::operator/(const double& scalar) const
    {
        double ret_x = x / scalar;
        double ret_y = y / scalar;
        return {ret_x, ret_y};
    }

    Vector2 operator+(double& scalar, const Vector2& vec)
    {
        double ret_x = scalar + vec.x;
        double ret_y = scalar + vec.y;
        return {ret_x, ret_y};
    }

    Vector2 operator-(double& scalar, const Vector2& vec)
    {
        double ret_x = scalar - vec.x;
        double ret_y = scalar - vec.y;
        return {ret_x, ret_y};
    }

    Vector2 operator*(double& scalar, const Vector2& vec)
    {
        double ret   = scalar * vec.x;
        double ret_y = scalar * vec.y;
        return {ret, ret_y};
    }

    std::string Vector2::ToString() const
    {
        return std::string("( " + std::to_string(x) + " , " + std::to_string(y) + " )");
    }

    std::ostream& operator<<(std::ostream& out_stream, const Vector2& vec)
    {
        return out_stream << vec.ToString();
    }
} // namespace ienium