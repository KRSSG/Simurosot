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
    //Util::// LoggertoStdOut("Going to point : (%d , %d) from (%d, %d)",dpoint.x,dpoint.y, state->homePos[botID].x, state->homePos[botID].y);
    float dist = (state->homePos[botID] - Vector2D<int>(dpoint)).absSq();
		_goToPointDW(botID, dpoint, finalvel, param.GoToPointP.finalslope);
  
  }
}
