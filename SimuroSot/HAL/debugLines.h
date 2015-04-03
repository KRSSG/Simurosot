#pragma once
#include <algorithm>
#include <stdio.h>
#include <stdint.h>

namespace HAL
{

class Debug_Line 
{
 public:
  Debug_Line();
  virtual ~Debug_Line(){};
  
  inline uint32_t x1() const;
  inline void set_x1(uint32_t value);
  
  inline uint32_t y1() const;
  inline void set_y1(uint32_t value);
  
  // required uint32 x2 = 3;
  inline uint32_t x2() const;
  inline void set_x2(uint32_t value);
  
  // required uint32 y2 = 4;
  inline uint32_t y2() const;
  inline void set_y2(uint32_t value);
  
  // required uint32 color = 5;
  inline uint32_t color() const;
  inline void set_color(uint32_t value);
  
  // @@protoc_insertion_point(class_scope:Debug_Line)
 private:
  
  uint32_t x1_;
  uint32_t y1_;
  uint32_t x2_;
  uint32_t y2_;
  uint32_t color_;
  
  };
}