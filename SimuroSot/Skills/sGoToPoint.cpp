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




  void SkillSet::goToPoint(const SParam &param)
  {
    Vector2D<int> dpoint;
    float finalvel;
    dpoint.x  = param.GoToPointP.x;
    dpoint.y  = param.GoToPointP.y;
    finalvel  = param.GoToPointP.finalVelocity;
    //Util::// LoggertoStdOut("Going to point : (%d , %d) from (%d, %d)",dpoint.x,dpoint.y, state->homePos[botID].x, state->homePos[botID].y);
    float dist = (state->homePos[botID] - Vector2D<int>(dpoint)).absSq();
    if(!param.GoToPointP.align )//|| dist < 9 * BOT_BALL_THRESH * BOT_BALL_THRESH)
      // For testting changing bellow calss to _goToPointPolar, originally _goToPoint
		_goToPoint(botID, dpoint, finalvel, param.GoToPointP.finalslope, 0,param.GoToPointP.increaseSpeed,param.GoToPointP.trapezoidal);
    else
		_goToPoint(botID, dpoint, finalvel, param.GoToPointP.finalslope, CLEARANCE_PATH_PLANNER,param.GoToPointP.increaseSpeed,param.GoToPointP.trapezoidal);
  
  }
}
