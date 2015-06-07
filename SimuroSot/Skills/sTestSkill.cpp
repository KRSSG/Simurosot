#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <math.h>
#include <stdio.h>
namespace MyStrategy
{




  void SkillSet::testSkill(const SParam &param)
  {
    static int R = 0  ;
    Vector2D<int> centre = state->awayPos[state->oppBotNearestToBall] ;
	Vector2D<int> target;
	target.x = 0 ; target.y = 0 ;
	static bool firstRun = true ;
	if(firstRun)
	 {
		 R = abs((state->homePos[botID].y - centre.y)/cos(state->homeAngle[botID]));
	     firstRun = false ;
	 }
	int r = BOT_RADIUS ;
	float w = 0.6*MAX_BOT_SPEED / R ;
	int vl = w*(R-r) ;
	int vr = w*(R+r) ;
	comm->sendCommand(botID,vr,vl) ;
  
  }
}
