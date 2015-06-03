#pragma once
#ifndef COMM_H
#define COMM_H

#include <list>
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
    Util::CS*                   debug_cs;
	void sendCommand(int   botID,           // valid bot ID
                             float v_l,             // in mm/sec
                             float v_r             // in mm/sec
                             ) ;    // on(true) or off(false)
 }; // class Comm
 // namespace HAL

#endif // COMM_H
