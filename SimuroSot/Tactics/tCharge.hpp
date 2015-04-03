#ifndef TTCHARGE_HPP
#define TTCHARGE_HPP

#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"

//TODO: Make a tactic such as tStealAndShoot which steals the ball, then shoots it, and
//      continues the stealing a few times if a goal isn't scored.

namespace MyStrategy
{
  class TCharge : public Tactic
  {
  public:
    TCharge(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TCharge

    ~TCharge()
    { } // ~TCharge

    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
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
      if(state->pr_ourBall)
      {
        sID = SkillSet::DribbleToPoint;
        sParam.DribbleToPointP.x = HALF_FIELD_MAXX;
        sParam.DribbleToPointP.y = (OPP_GOAL_MAXY + OPP_GOAL_MINY) / 2;
        skillSet->executeSkill(sID, sParam);
      }
      else
      {
        captureBall(Vector2D<int>::angle(state->ballPos, state->homePos[botID]));
      }

      if (state->pr_ourGoalScored)
        tState = COMPLETED;
      else
        tState = RUNNING;
    }
  };
// class TCharge
} // namespace MyStrategy

#endif // TTCharge_HPP