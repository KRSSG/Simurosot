#ifndef TDEFEND_HPP
#define TDEFEND_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TDefend : public Tactic
  {
  public:
    TDefend(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TDefend

    ~TDefend()
    { } // ~TDefend

    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = fabs((float)(float)state->homePos[*(freeBots.begin())].x - tParam->DefendP.x);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_defendline = fabs((float)(float)state->homePos[*it].x - tParam->DefendP.x);
        if(*it == botID)
          dis_from_defendline -= sqrt((float)HYSTERESIS);
        if(dis_from_defendline < mindis)
        {
          dis_from_defendline = mindis;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      float x = tParam.DefendP.x;
      if(state->ballPos.x < x)
      {
        x = state->ballPos.x - 2 * BOT_BALL_THRESH;
      }
      gotoPointExact(x, state->ballPos.y,true, PI/2, 0);
      if(state->pr_ourBall)
        tState = COMPLETED;
      else
        tState = RUNNING;

    }
  }; // class TDefend
} // namespace MyStrategy

#endif // TDEFEND_HPP
