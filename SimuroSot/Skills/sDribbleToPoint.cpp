#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <math.h>
#include <stdio.h>

#define SGN(x) (((x)>0)?1:(((x)<0)?(-1):0))
#define CLEARANCE 80.0
#define MIN_DIST_FROM_TARGET 150.0

namespace MyStrategy
{
  void SkillSet::dribbleToPoint(const SParam &param)
  {
    Vector2D<int> dpoint;
    dpoint.x = param.DribbleToPointP.x;
    dpoint.y = param.DribbleToPointP.y;

    obstacle obs[HomeTeam::SIZE + AwayTeam::SIZE];
    for (int i = 0; i < HomeTeam::SIZE; ++i)
    {
      obs[i].x = state->homePos[i].x;
      obs[i].y = state->homePos[i].y;
      obs[i].radius = 3 * BOT_RADIUS;
    }

    for (int i = HomeTeam::SIZE; i < HomeTeam::SIZE + AwayTeam::SIZE; ++i)
    {
      obs[i].x = state->awayPos[i - HomeTeam::SIZE].x;
      obs[i].y = state->awayPos[i - HomeTeam::SIZE].y;
      obs[i].radius = 3 * BOT_RADIUS;
    }
    Vector2D<int> point, nextWP, nextNWP;
#ifdef LOCAL_AVOID
    pathPlannerSec->plan(state->homePos[botID],
                      dpoint,
                      &nextWP,
                      &nextNWP,
                      obs,
                      HomeTeam::SIZE + AwayTeam::SIZE,
                      botID,
                      true);
#else
	pathPlanner->plan(state->homePos[botID],
                      dpoint,
                      &nextWP,
                      &nextNWP,
                      obs,
                      HomeTeam::SIZE + AwayTeam::SIZE,
                      botID,
                      true);
#endif

    if (nextWP.valid())
    {
      comm->addCircle(nextWP.x, nextWP.y, 50);
      comm->addLine(state->homePos[botID].x, state->homePos[botID].y, nextWP.x, nextWP.y);
    }
    if (nextNWP.valid())
    {
      comm->addCircle(nextNWP.x, nextNWP.y, 50);
      comm->addLine(nextWP.x, nextWP.y, nextNWP.x, nextNWP.y);
    }
    float phiStar, theta = normalizeAngle(Vector2D<int>::angle(nextWP, state->homePos[botID]));	
    if(nextNWP.valid())
      phiStar = Vector2D<int>::angle(nextNWP, nextWP);
    else
      phiStar = param.GoToPointP.finalslope;
    float phi = (state->homeAngle[botID] < -PI)? PI:state->homeAngle[botID];

    float dist = Vector2D<int>::dist(nextWP, state->homePos[botID]);  // Distance of next waypoint from the bot
    float alpha=normalizeAngle(theta - phiStar);
    float beta;

    if(fabs((float)alpha)<fabs((float)atan2(CLEARANCE, dist)))
      printf("ALPHA!\n");
    else
      printf("c/d\n");
	
    beta=(fabs((float)alpha)<fabs((float)atan2(CLEARANCE,dist)))?(alpha):SGN(alpha)*atan2(CLEARANCE,dist);

    float thetaD=normalizeAngle(theta + beta);
    float delta=normalizeAngle(thetaD-phi);
    float r= ROTATION_FACTOR*(sin(delta)*sin(delta)*SGN(sin(delta)));
    float t= (cos(delta)*cos(delta));//SGN(tan(delta)); 
    float ballBotAngle=fabs((float)normalizeAngle(Vector2D<int>::angle(state->ballPos,state->homePos[botID])-state->homeAngle[botID]));
    if(ballBotAngle>PI/2)
    {
      t=-t;
      r=-r;
    }
	
    printf("delta  = %f\n", ballBotAngle*180/PI);	

    float profileFactor;
    float bot_ball_dist=Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
	
    profileFactor = MAX_BOT_SPEED/2;

    printf("dist= %f\n", dist);

    if((bot_ball_dist < BOT_BALL_THRESH)/*&& (fabs((float)state->homeAngle[botID]-finalSlope)<ANGLECLEARANCE)*/)
    {
#if FIRA_COMM || FIRASSL_COMM
      comm->sendCommand(botID, profileFactor*(t+r), profileFactor*(t-r));        
#elif  GR_SIM_COMM
      comm->sendCommand(botID, profileFactor*(t-r), profileFactor*(t+r));
#endif
    }
    else
    {
      comm->sendCommand(botID,0,0);
    }
  } // goToPoint
}
