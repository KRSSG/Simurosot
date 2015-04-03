#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include <math.h>
#include <stdio.h>
#define DEFEND_RADIUS 2000.0

namespace MyStrategy
{
  void SkillSet::defendPoint(const SParam& param)
  {
    float point_ball_angle = atan2(state->ballPos.y-param.DefendPointP.y,state->ballPos.x-param.DefendPointP.x);
    Vector2D<int> dpoint;
    printf("Ball : %d, %d\n", state->ballPos.x, state->ballPos.y);
    dpoint.y=param.DefendPointP.y + DEFEND_RADIUS*sin( point_ball_angle);
    dpoint.x=param.DefendPointP.x + DEFEND_RADIUS*cos( point_ball_angle);
    _goToPoint(botID,dpoint,0,point_ball_angle,0);
  } 
}
