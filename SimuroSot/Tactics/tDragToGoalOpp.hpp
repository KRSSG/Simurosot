#ifndef TTDRAGTOGOALOPP_HPP
#define TTDRAGTOGOALOPP_HPP


#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

//TODO: Make a tactic such as tStealAndShoot which steals the ball, then shoots it, and 
//      continues the stealing a few times if a goal isn't scored.

namespace MyStrategy
{
  class TDragToGoalOpp : public Tactic
  {
  public:
    TDragToGoalOpp(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TCharge

    ~TDragToGoalOpp()
    { } // ~TCharge

    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = (state->homePos[*it] - state->ballPos).absSq();
        if(dis_from_ball < mindis)
        {
          mindis = dis_from_ball;
          minv = *it;
        }
      }
     // Util::// LoggertoStdOut("Selected bot %d\n", minv);
      return minv;
      
    } // chooseBestBot
    
    void execute(const Param& tParam)
    {
      // Select the skill to be executed next
      #undef HALF_FIELD_MAXX
      #define HALF_FIELD_MAXX -SELECT(3025, 225)

      Vector2D<int> fastBallPos  = Vector2D<int>((int)state->ballPos.x + 0.3*state->ballVel.x, (int)state->ballPos.y + 0.3*state->ballVel.y);
      Vector2D<int> goalPoint(HALF_FIELD_MAXX, 0);
      float theta               = normalizeAngle(Vector2D<int>::angle(goalPoint, fastBallPos));
      float angleWithBall       = normalizeAngle(Vector2D<int>::angle(fastBallPos, state->homePos[botID]));
      Vector2D<int> finalPoint  = state->homePos[botID] + Vector2D<int>(2*BOT_BALL_THRESH*cos(theta), 2*BOT_BALL_THRESH*sin(theta));
      
      Vector2D<int> targetPoint = fastBallPos - Vector2D<int>(2*BOT_BALL_THRESH*cos(theta), 2*BOT_BALL_THRESH*sin(theta));
      float dist = Vector2D<int>::dist(targetPoint, state->homePos[botID]);
      printf("Drag to Goal: %f, %d, %f, \n", dist, 2*BOT_BALL_THRESH, fabs((float)firaNormalizeAngle(theta)));
      float sign = (state->homePos[botID].x - fastBallPos.x)*(state->homePos[botID].x - HALF_FIELD_MAXX);
      static int stt = 1;
      printf("State: %d\n", stt);
      if(stt == 0 && dist < 2*BOT_BALL_THRESH && fabs((float)firaNormalizeAngle(theta - angleWithBall)) < 0.05 && sign > 0) {
        stt = 1;
      }
      if(stt == 1 && dist > 3*BOT_BALL_THRESH || sign < 0) {
        stt = 0;
      }
      if(stt == 1)
      {
       // Util::// LoggertoStdOut("Dribbling to goal %d %d", goalPoint.x, goalPoint.y);
        // BUG finalVelocity is 0.1?? what is this?
        gotoPoint(goalPoint.x, goalPoint.y, true, normalizeAngle(Vector2D<int>::angle( goalPoint, fastBallPos )), 0.1);
      }
      else{
       // Util::// LoggertoStdOut("Going to ball");
        gotoPoint(targetPoint.x, targetPoint.y, true, Vector2D<int>::angle(goalPoint, state->homePos[botID]), 0);
      }
      if (state->pr_oppBall || state->pr_looseBall/*||state->pr_goalscored*/)
      {
//      tState = COMPLETED;
      }
      #undef HALF_FIELD_MAXX
      #define HALF_FIELD_MAXX SELECT(3025, 225)
    }
  }; // class TCharge
} // namespace MyStrategy

#endif // TTCharge_HPP
