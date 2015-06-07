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
 void SkillSet::goToPointDW(const SParam &param)
 {
   Vector2D<int> dpoint;
   float finalvel = 0 ;
   dpoint.x  = param.GoToPointDWP.x;
   dpoint.y  = param.GoToPointDWP.y;
float dist = Vector2D<int>::dist(state->homePos[botID],dpoint ) ;
if(dist < 1.5*BOT_BALL_THRESH)
comm->sendCommand(botID,0,0) ;
else
_goToPointDW(botID, dpoint, finalvel, param.GoToPointP.finalslope);

 
 }
}
