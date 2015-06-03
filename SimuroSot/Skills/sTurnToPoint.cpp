//new turn to pt to show jm
#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <stdio.h>

namespace MyStrategy
{
  void SkillSet::turnToPoint(const SParam& param)
  {
    float vl,vr;
    Vector2D<int> nextWP(param.TurnToPointP.x,param.TurnToPointP.y);
    float finalSlope = atan2(param.TurnToPointP.y-state->homePos[botID].y,param.TurnToPointP.x-state->homePos[botID].x);
    float dist = Vector2D<int>::dist(nextWP, state->homePos[botID]); 
    float turnAngleLeft = normalizeAngle(finalSlope - state->homeAngle[botID]); // Angle left to turn
    float profileFactor= MAX_BOT_SPEED*dist/ MAX_FIELD_DIST;
    if(turnAngleLeft>PI/2||turnAngleLeft<-PI/2)
    {
      profileFactor=-profileFactor;
      if(turnAngleLeft>PI/2)
        turnAngleLeft=turnAngleLeft-PI;
      else
        turnAngleLeft=turnAngleLeft+PI;
    }
    float omega = turnAngleLeft * MAX_BOT_OMEGA/(16*PI); // Speedup turn
    

    vr = -(omega * (MAX_BOT_SPEED))/MAX_BOT_OMEGA ;
    vl = -vr;
    //comm.sendCommand(botID, 0,(vl + vr)/2, ((vl - vr)*MAX_BOT_OMEGA)/(2*MAX_BOT_SPEED), 0, false);
    
    printf("turnangle left %f",turnAngleLeft);
    //printf("State Home pos: %d %d", state->homePos[botID].x,state->homePos[botID].y);
    if (fabs((float)turnAngleLeft) < 0.1 && dist < BOT_POINT_THRESH)
      comm->sendCommand(botID,0,0);
    else if(fabs((float)turnAngleLeft) < 0.1)
    {
      printf("prffact: %f\n",profileFactor);
      comm->sendCommand(botID,profileFactor,profileFactor);
    }      
    else
    {
      printf("vl=%f, \nvr=%f \n,omega=%f \n",vl,vr,omega);
      comm->sendCommand(botID,vl,vr);
    }
      
   
   }
}

//old turn to pt
/*#include "skillSet.h"
#include "../../Core/beliefState.h"
#include "config.h"

namespace MyStrategy
{
  void SkillSet::turnToPoint(const SParam& param)
  {
    float vl,vr;
    Vector2D<int> point(param.TurnToPointP.x, param.TurnToPointP.y);
    float finalSlope = Vector2D<int>::angle(point, state->homePos[botID]);
    float turnAngleLeft = normalizeAngle(finalSlope - state->homeAngle[botID]); // Angle left to turn
    if(turnAngleLeft>PI/2||turnAngleLeft<-PI/2)
    {
      if(turnAngleLeft>PI/2)
        turnAngleLeft=turnAngleLeft-PI;
      else
        turnAngleLeft=turnAngleLeft+PI;
    }
    float omega = turnAngleLeft * MAX_BOT_OMEGA / (2 * PI); // Speedup turn
    if(omega < MIN_BOT_OMEGA && omega > -MIN_BOT_OMEGA)
    {
      if(omega < 0) omega = -MIN_BOT_OMEGA;
      else omega = MIN_BOT_OMEGA;
    }
    comm.addCircle(state->homePos[botID].x,  state->homePos[botID].y, 50);
    float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
    //comm.sendData(botID,vl,vr);
    vr = (omega * (MAX_BOT_SPEED))/MAX_BOT_OMEGA ;
    vl = -vr;
    //comm.sendCommand(botID, 0,(vl + vr)/2, ((vl - vr)*MAX_BOT_OMEGA)/(2*MAX_BOT_SPEED), 0, false); 
    comm.sendCommand(botID,vl,vr);
       
  }
}*/
