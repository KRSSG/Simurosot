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

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000;
      for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = (state->homePos[*it] - state->ballPos).absSq();
        //cannot use below condition since tparam is not passed as paramater
        //float dis_from_ball = (state->homePos[*it] - tParam.DefendPointP).absSq();
        if(dis_from_ball < mindis)
        {
          mindis = dis_from_ball;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

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
