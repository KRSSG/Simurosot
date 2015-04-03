//new turn to pt to show jm
#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include "../winDebugger/Client.h"
#define KD_ANGLE 0.1
namespace MyStrategy
{
  void SkillSet::chargeBall(const SParam& param)
  {
    float vl,vr;
    float finalSlope = atan2(state->ballPos.y-state->homePos[botID].y,state->ballPos.x-state->homePos[botID].x);
    float turnAngleLeft = normalizeAngle(finalSlope - state->homeAngle[botID]); // Angle left to turn

	bool botInverted = false;
    if(turnAngleLeft>PI/2||turnAngleLeft<-PI/2)
    {
	  botInverted = true;
      if(turnAngleLeft>PI/2)
        turnAngleLeft=turnAngleLeft-PI;
      else
        turnAngleLeft=turnAngleLeft+PI;
    }
    
    //float omega = turnAngleLeft * MAX_BOT_OMEGA/(16*PI); // Speedup turn
    float factor = (turnAngleLeft+KD_ANGLE*(turnAngleLeft))/(PI/2);
    vr = -0.4*MAX_BOT_SPEED*(factor)/(PI/2);
    //vr = -MAX_BOT_SPEED*(turnAngleLeft)/(PI/2);
    vl = -vr;
    
    if(fabs(turnAngleLeft) > DRIBBLER_BALL_ANGLE_RANGE) {
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, vl, vr);        
#else
    comm->sendCommand(botID, vr, vl);
#endif
    }else {
		if(botInverted)
			comm->sendCommand(botID, -100, -100);
		else
			comm->sendCommand(botID, 100, 100);
    }
   
   }
}
