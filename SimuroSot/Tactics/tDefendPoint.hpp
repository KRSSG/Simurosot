#ifndef TDEFENDPOINT_HPP
#define TDEFENDPOINT_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TDefendPoint : public Tactic
  {
  public:
    TDefendPoint(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TDefendPoint

    ~TDefendPoint()
    { } // ~TDefendPoint

    inline bool isActiveTactic(void) const
    {
      return false;
    }


    void execute(const Param& tParam)
    {
      printf("\n%f\n", Vector2D<int>::dist(state->ballPos, state->homePos[botID]));


      {
        // Select the skill to the executed next
        //sID = SkillSet::DefendPoint;
        sParam.DefendPointP.x = tParam.DefendPointP.x;
        sParam.DefendPointP.y = tParam.DefendPointP.y;
        //sParam.DefendPointP.radius = 200;
        // Execute the selected skill
        //skillSet->executeSkill(sID, sParam);
      }
      /*
      if (state->pr_ourGoal||state->pr_oppGoal||state->pr_ourFreeKick||state->pr_ourPenalty)
      {
        tState = COMPLETED;
      }
       * */
    }
  } ;// class TDefendPoint
} // namespace MyStrategy

#endif // TDEFENDPOINT_HPP
