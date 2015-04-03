#include "stdafx.h"

#include "debugCircle.h"

namespace HAL
{
inline void Debug_Circle::set_radius(uint32_t value)
{
	this->radius_=value;
}

inline uint32_t Debug_Circle::color() const
{
	return this->color_;
}

inline void Debug_Circle::set_color(uint32_t value)
{
	this->color_ = value;
}

inline uint32_t Debug_Circle::x() const
{
	return this->x_;
}

inline void Debug_Circle::set_x(uint32_t value)
{
	this->x_ = value;
}

inline uint32_t Debug_Circle::y() const
{
	return this->y_;
}

inline void Debug_Circle::set_y(uint32_t value)
{
	this->y_ = value;
}

inline uint32_t Debug_Circle::radius() const
{
	return this->radius_;
}

}
