#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#define ANGLE_THRESH 0.1
using namespace std;

namespace MyStrategy
{
  void SkillSet::goToBall(const SParam& param)
  {
		Vector2D<int> ballInitialpos,ballFinalpos;
		ballInitialpos = state->ballPos;
		ballFinalpos.x = state->ballPos.x+(0.7*state->ballVel.x);
		ballFinalpos.y = state->ballPos.y+(0.7*state->ballVel.y);
		
		float botballdist = Vector2D<int>::dist(ballInitialpos,state->homePos[botID]);
		float botballVeldist = Vector2D<int>::dist(ballFinalpos,state->homePos[botID]);
		
		float theta=Vector2D<int>::angle(ballInitialpos,ballFinalpos);
		
		if (param.GoToBallP.align==true)
		{
			if((Vector2D<int>::dist(ballInitialpos,state->homePos[botID]))>=BOT_BALL_THRESH)
			{
					if(fabs((float)tan(Vector2D<int>::angle(ballInitialpos,ballFinalpos))-tan(Vector2D<int>::angle(state->homePos[botID],ballFinalpos)))<ANGLE_THRESH)
            _goToPoint(botID,ballInitialpos,0,theta,50.0);
          else
            _goToPoint(botID,ballFinalpos,0,theta,50.0);
			}
			else 
			{
//        float vl,vr;
//        //float finalSlope = Vector2D<int>::angle(ballInitialpos,ballFinalpos);
//        float turnAngleLeft = normalizeAngle(theta - state->homeAngle[botID]); // Angle left to turn
//        if(turnAngleLeft>PI/2||turnAngleLeft<-PI/2)
//        {
//          if(turnAngleLeft>PI/2)
//            turnAngleLeft=turnAngleLeft-PI;
//          else
//            turnAngleLeft=turnAngleLeft+PI;
//        }
//        float omega = turnAngleLeft * MAX_BOT_OMEGA/(16*PI); // Speedup turn
//        vr = -MAX_BOT_SPEED*turnAngleLeft/(2*PI);
//        vl = -vr;
//        printf("vl=%f, \nvr=%f \n,omega=%f \n",vl,vr,omega);
//        printf("State home angle: %f",state->homeAngle[botID]);
//        printf("State Home pos: %d %d", state->homePos[botID].x,state->homePos[botID].y);
//
//        if(fabs((float)turnAngleLeft) < 0.3)
//          comm.sendCommand(botID,0,0);
//        else
          comm->sendCommand(botID,0,0);
			}
		}
		else
		{
			if((Vector2D<int>::dist(ballInitialpos,state->homePos[botID]))>=BOT_BALL_THRESH)
			{
					_goToPoint(botID,ballInitialpos,0,theta,00.0);
      }
			else 
			{
				comm->sendCommand(botID,0,0);
			}
		}
  } // goToBall
}
