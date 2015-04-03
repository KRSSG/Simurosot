#pragma once
#include <algorithm>
#include <stdio.h>
#include <stdint.h>

namespace HAL
{
class Debug_Circle 
{
 public:
  Debug_Circle();
   ~Debug_Circle(){};
  
  inline uint32_t x() const;
  inline void set_x(uint32_t value);
 
  inline uint32_t y() const;
  inline void set_y(uint32_t value);
  
  inline uint32_t radius() const;
  inline void set_radius(uint32_t value);
  
  inline uint32_t color() const;
  inline void set_color(uint32_t value);
  
  private:
  
	uint32_t x_;
	uint32_t y_;
	uint32_t radius_;
	uint32_t color_;
  
};
}