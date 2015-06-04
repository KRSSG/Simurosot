#ifndef TGOALKEEPINGOPP_HPP
#define TGOALKEEPINGOPP_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
#define GOAL_Y 0
#define AWAYGOAL_X HALF_FIELD_MAXX
namespace MyStrategy
{
  class TGoalKeepingOppSide : public Tactic
  {
  public:
    int state1;
    TGoalKeepingOppSide(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
      state1 = 0;
    } // TGoalKeeping

    ~TGoalKeepingOppSide()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }


    void execute(const Param& tParam)
    {
      printf("Ball Pos: %d %d\n", state->ballPos.x, state->ballPos.y);
      if (!isGoalKeeperInPosition() && state1 == 0)
      {
        //Bot elsewhere in field. Bring the bot to the Goalkeeper position.
        //Util::// LoggertoStdOut("Bot going to goalkeeper positon.");
        gotoPointExact((HALF_FIELD_MAXX) - DBOX_WIDTH / 6, (OUR_GOAL_MAXY + OUR_GOAL_MINY) / 2, true, PI/2, MIN_BOT_SPEED);
      }
      else
      {
        //Bot in Goalkeeper position. Show goalkeeping skills.
        state1 = 1 ;
        gotoPointExact((HALF_FIELD_MAXX) - DBOX_WIDTH / 6, getBotDestPointY(), true, PI/2, 0);
       // Util::// LoggertoStdOut("Bot showing goalkeeping skills (%f,%f).\n", sParam.GoToPointP.x, sParam.GoToPointP.y);
      }
      tState = COMPLETED;

    }

    bool isGoalKeeperInPosition()
    {
      //Util::// LoggertoStdOut("Half_field_max : %d, Dbox_width : %d\n",HALF_FIELD_MAXX,DBOX_WIDTH);
      if ((state->homePos[botID].x < HALF_FIELD_MAXX) &&
          (state->homePos[botID].x > HALF_FIELD_MAXX - DBOX_WIDTH) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY) &&
          (state->homePos[botID].y <= OUR_GOAL_MAXY))
        return true;
      else
        return false;

    }

    int getBotDestPointY()
    {
      Vector2D<int> ballFinalpos, botDestination, point;
      ballFinalpos.x = state->ballPos.x + state->ballVel.x;
      ballFinalpos.y = state->ballPos.y + state->ballVel.y;
      botDestination.x = HALF_FIELD_MAXX - DBOX_WIDTH / 2;

      /* Workaround for ball velocity 0*/
      if(fabs((float)(float)ballFinalpos.y - state->ballPos.y) < 5 || fabs((float)(float)ballFinalpos.x - state->ballPos.x) < 5)
      {
       // Util::// LoggertoStdOut("******************Ball velocity is zero.\n");
        point.x = (HALF_FIELD_MAXX) - DBOX_WIDTH / 2;
        point.y = state->ballPos.y;
      }
      else
      {
        botDestination.y = (ballFinalpos.y - state->ballPos.y) / (ballFinalpos.x - state->ballPos.x) * (botDestination.x - ballFinalpos.x) + ballFinalpos.y;
        point.x = (HALF_FIELD_MAXX) - DBOX_WIDTH / 2;;
        point.y = botDestination.y;
        point.y = state->ballPos.y;
      }
      /* Set Limits on y to not exceed DBOX Y Limits*/
      if(point.y < OUR_GOAL_MINY)
        point.y = OUR_GOAL_MINY + BOT_RADIUS;
      else if(point.y > OUR_GOAL_MAXY)
        point.y = OUR_GOAL_MAXY - BOT_RADIUS;

      return point.y;
    }
  } ;// class TGoalKeeping
} // namespace MyStrategy

#endif // TGOALKEEPING_HPP
