#include "stdafx.h"
#include <cassert>
#include "comm.h"
#include "../common/include/cs.hpp"
#include "../Strategy.h"
#include <cmath>
#define PI 3.141592653589793
using namespace Util;
using namespace HAL;

  CS   Comm::commCS;  
Comm* Comm::singletonInstance=0;
  Comm::Comm() {}

  Comm* Comm::getInstance()
  {
	  if(singletonInstance==0)
	  {
		  singletonInstance = new Comm();
	  }
	  return singletonInstance;

  }
  Comm::~Comm()
  { }  
  

  void Comm::addCircle(int x, int y, unsigned int radius, unsigned int color){}
  void Comm::addLine(int x1, int y1, int x2, int y2, unsigned int color ){}
  void Comm::addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color ){}
  void Comm::addAdjustBot(int team ,int id, int x, int y, float dir, int vx, int vy, int vz){}
  void Comm::addAdjustBall(int x, int y, int vx, int vy, int vz){}
     
     void Comm::sendCommand(int   botID,           // valid bot ID
                             float v_l,             // in mm/sec
                             float v_r             // in mm/sec
							 )  // on(true) or off(false)

	 {
		// assert (fabs(v_l)>125 ||fabs(v_r)>125);
		 envi->home[botID].velocityLeft = v_l;
		 envi->home[botID].velocityRight = v_r;
	 }
  std::list<HAL::Debug_Circle> Comm::getCircles() { 
    debug_cs->enter();
    std::list<Debug_Circle> temp = debug_circles;
    debug_cs->leave();
    return temp;
  }
  std::list<HAL::Debug_Line> Comm::getLines() {
    debug_cs->enter();
    std::list<Debug_Line> temp = debug_lines;
    debug_cs->leave();
    return temp;
  }
 // namespace HAL
