#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <stdio.h>
using namespace Util;
#define RBALL 3  //3
#define LBOT 150 //100
#define LGOAL 800
#define PF(x) (state->ballPos.y<=x.y)?-1:1
#define SGN(x) (x<0)?-1:((x==0)?0:1))
namespace MyStrategy
{
  void SkillSet::spinToGoal(const SParam& param)
  {
    Vector2D<int> point[7];   //p1 vector joining ball and upper goal pole,p2 is vector joining ball and lower goal pole
    point[0].x=3000,point[0].y=LGOAL/2;
    point[1].x=3000,point[1].y=LGOAL/3;
    point[2].x=3000,point[2].y=LGOAL/6;
    point[3].x=3000,point[3].y=0;
    point[4].x=3000,point[4].y=-LGOAL/6;
    point[5].x=3000,point[5].y=-LGOAL/3;
    point[6].x=3000,point[6].y=-LGOAL/2;
    obstacle obs[HomeTeam::SIZE + AwayTeam::SIZE];
    for (int i = 0; i < HomeTeam::SIZE; ++i)
    {
      obs[i].x = state->homePos[i].x;
      obs[i].y = state->homePos[i].y;
      obs[i].radius = 2 * BOT_RADIUS;
    }

    for (int i = HomeTeam::SIZE; i < HomeTeam::SIZE + AwayTeam::SIZE; ++i)
    {
      obs[i].x = state->awayPos[i - HomeTeam::SIZE].x;
      obs[i].y = state->awayPos[i - HomeTeam::SIZE].y;
      obs[i].radius = 2 * BOT_RADIUS;
    }
    float theta;
    Vector2D<int> nextWP, nextNWP,centre;
    for(int x=0;x<7;x++)
    {
#ifdef LOCAL_AVOID
      pathPlannerSec->plan(state->ballPos,
                      point[x],
                      &nextWP,
                      &nextNWP,
                      obs,
                      HomeTeam::SIZE + AwayTeam::SIZE,
                      botID,
                      true);
#else
	 pathPlanner->plan(state->ballPos,
                      point[x],
                      &nextWP,
                      &nextNWP,
                      obs,
                      HomeTeam::SIZE + AwayTeam::SIZE,
                      botID,
                      true);
#endif
      theta=Vector2D<int>::angle(point[x],state->ballPos); //vector joining ball pos and goal point
      if(Vector2D<int>::dist(point[x],nextWP)<10)
      {
        printf("%d\n",x);
        centre.x=state->ballPos.x-(RBALL*cos(theta)+(LBOT/2)*sin(theta)+(LBOT/2)*cos(theta));
        centre.y=state->ballPos.y-RBALL*sin(theta)+(LBOT/2)*cos(theta)-(LBOT/2)*sin(theta);

        if(Vector2D<int>::dist(state->homePos[botID],centre)>BOT_POINT_THRESH)
        {
          _goToPoint(botID,centre,0,theta + PI/2,250.0);
          printf("\nreturn called");
          return;
        }

        comm->sendCommand(botID,-MAX_BOT_SPEED, MAX_BOT_SPEED);
        for(int x=0;x<100000;x++)   //delay loop
            printf("\nhitturn");
        return;
      }
    }
  }
}  
