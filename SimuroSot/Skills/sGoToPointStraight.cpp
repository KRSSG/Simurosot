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
  #define BOT_POINT_ANGLE_THRESHOLD 0.2
void SkillSet::goToPointStraight(const SParam &param)
	{
		Vector2D<int> dest;
    dest.x = param.GoToPointP.x;
    dest.y = param.GoToPointP.y;
    _goToPointStraight(botID,dest,param.GoToPointP.finalVelocity,param.GoToPointP.finalslope,0.0);
	}
}
