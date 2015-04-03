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
void SkillSet::goToBallStraight(const SParam &param)
	{
		Vector2D<int> dest;
    dest.x = state->ballPos.x;
    dest.y = state->ballPos.y;
    float vl =0,vr = 0;
    float dist = Vector2D<int>::dist(dest, state->homePos[botID]);   
		float theta = normalizeAngle(Vector2D<int>::angle(dest, state->homePos[botID]));
    float init_angle = normalizeAngle(theta - state->homeAngle[botID]);
    //Util::// LoggertoStdOut("BotPos  : ( %d, %d )\n",state->homePos[botID].x, state->homePos[botID].y);
    //Util::// LoggertoStdOut("Dist  : %f, Init_Angle  : %f\n",dist,init_angle);
    if(dist > BOT_BALL_THRESH && fabs((float)init_angle) > 0.5)
    {
      /* Rotate in place to move forward.*/
      _turnToAngle(init_angle,&vl,&vr);
    }
    else if(dist > BOT_BALL_THRESH && fabs((float)init_angle) <=0.5)
    {
      /* Bot aligned correctly. Move forward */
      //Util::// LoggertoStdOut("Moving forward by : %f\n",MAX_BOT_SPEED);
      float profileFactor = (dist < 500)? 0.3 : dist/(2*HALF_FIELD_MAXX);
      float v = profileFactor*MAX_BOT_SPEED;
      vl = vr = fabs((float)v)<MIN_BOT_SPEED ? MIN_BOT_SPEED : v;
    }
    else if(dist < BOT_BALL_THRESH && param.GoToPointP.align)
    {
      /* Bot has reached desired location.*/
      float final_angle = normalizeAngle(param.GoToPointP.finalslope - state->homeAngle[botID]);
      if(fabs((float)final_angle) > 0.1)
      {
        /*Bot is not aligned to final slope angle. Turn it.*/
        _turnToAngle(final_angle,&vl,&vr);
      }
      else
      {
        /* Bot is aligned to final slope angle. Stop */
         //Util::// LoggertoStdOut("Bot has reached destination.");
         vl = vr = 0;
      }
    }
    else
    {
      //Util::// LoggertoStdOut("No align required.");
      vl = vr = 0;
    }
    
    comm->sendCommand(botID,vl,vr);    
	}
}
