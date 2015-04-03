#ifndef TTPASS_HPP
#define TTPASS_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
#define D_TPASS 1
namespace MyStrategy
{
  class TPass : public Tactic
  {
  public:
    TPass(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {} // TPass

    ~TPass()
    { } // ~TPass

    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = Vector2D<int>::dist(state->homePos[minv], state->ballPos);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dist_from_ball = Vector2D<int>::dist(state->homePos[*it], state->ballPos);
        if(dist_from_ball < mindis)
        {
          mindis = dist_from_ball;
          minv = *it;
        }
      }
      //debug(1, minv, "assigned Pass");
      return minv;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
      //tState = RUNNING;
      if(state->pr_ourBall)
      {
        kickForGoalOrPassBall();
      } else {
        captureBall(Vector2D<int>::angle(Vector2D<int>(ForwardX(HALF_FIELD_MAXX), 0), state->ballPos));
      }
      if(state->pr_oppBall) 
        if(tState == RUNNING) tState = ABORTED;
    }
  };// class TPass
} // namespace MyStrategy

#endif // TTPass_HPP
