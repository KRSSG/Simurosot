#include "stdafx.h"
#include <algorithm>
#include <stdio.h>
#include <stdint.h>
#include "debugLines.h"

namespace HAL
{

	inline uint32_t Debug_Line::x1() const
	{
		return this->x1_;
	}
	inline void Debug_Line::set_x1(uint32_t value)
	{
		this->x1_ = value;
	}
	inline uint32_t Debug_Line::y1() const
	{
		return this->y1_;
	}
	inline void Debug_Line::set_y1(uint32_t value)
	{
		this->y1_ = value;
	}
		inline uint32_t Debug_Line::x2() const
	{
		return this->x2_;
	}
	inline void Debug_Line::set_x2(uint32_t value)
	{
		this->x2_ = value;
	}
	inline uint32_t Debug_Line::y2() const
	{
		return this->y2_;
	}
	inline void Debug_Line::set_y2(uint32_t value)
	{
		this->y2_ = value;
	}    
  // required uint32 color = 5;
	inline uint32_t Debug_Line::color() const
	{
		return this->color_;
	}
	inline void Debug_Line::set_color(uint32_t value)
	{
		this->color_ = value;
	}
  
}