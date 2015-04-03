#pragma once
#ifndef COMM_H
#define COMM_H

#include <list>
#include "debugLines.h"
#include "debugCircle.h"
#include "../Strategy.h"
// Forward Declarations

namespace Util
{
  class CS;
}

namespace MyStrategy
{
  class BeliefState;
}
class Comm
{
  private:
   
	Comm();	
	static Comm* singletonInstance;
  protected:
    static Util::CS commCS;
	
  public:
	   Environment *envi;
	static Comm* getInstance();
	virtual ~Comm();
    std::list<HAL::Debug_Circle>     debug_circles;
    std::list<HAL::Debug_Line>       debug_lines;
    Util::CS*                   debug_cs;
    /* This function must be implemented in the derived classes.
     * Remember to guard the entire function body with the CS object commCS
     * otherwise the result will be undefined and communication protocols
     * may break down
     */
     //Making addCircle and addLine as purely virtual, since these debug symbols really 
     void addCircle(int x, int y, unsigned int radius, unsigned int color = 0xFFFFFFFF);
     void addLine(int x1, int y1, int x2, int y2, unsigned int color = 0xFFFFFFFF);
	 void addAdjustBot(int team ,int id, int x, int y, float dir, int vx, int vy, int vz);
     void addAdjustBall(int x, int y, int vx, int vy, int vz);
     void addArrow(int to_x, int to_y, int from_x, int from_y, unsigned int color = 0xFFFFFFFF);
     void sendCommand(int   botID,           // valid bot ID
                             float v_l,             // in mm/sec
                             float v_r             // in mm/sec
                             ) ;    // on(true) or off(false)
    //Following provided for QtDebugger compatibiltity
    std::list<HAL::Debug_Circle> getCircles();
    std::list<HAL::Debug_Line> getLines();
    void clearDebugData() { debug_circles.clear(); debug_lines.clear();}
 }; // class Comm
 // namespace HAL

#endif // COMM_H
