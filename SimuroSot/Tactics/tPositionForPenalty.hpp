#ifndef TPOSITIONFORPENALTY_HPP
#define TPOSITIONFORPENALTY_HPP
#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
namespace MyStrategy
{
  class TPositionForPenalty : public Tactic
  {
  public:
    TPositionForPenalty(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TPositionForPenalty
    ~TPositionForPenalty()
    { } // ~TPositionForPenalty
    inline bool isActiveTactic(void) const
    {
      return false;
    }
    //CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
    //These values need to be fed into the tparam
    //this code snippet will allow you to the place the bot at the required position...
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        //this tactic does not require too much of sophistication for the tactic and the bot which is at the minimum  distance from the point is the best bot to do the job...
        float dis_from_point = (state->homePos[*it] - state->ballPos).absSq();
        if(dis_from_point < mindis)
        {
          dis_from_point = mindis;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
      // Select the skill to the executed next
      sID = SkillSet::GoToPoint;
      sParam.GoToPointP.x               = tParam.PositionForPenaltyP.x ;
      sParam.GoToPointP.y               = tParam.PositionForPenaltyP.y;
      sParam.GoToPointP.align           = true;
      sParam.GoToPointP.finalslope      = tParam.PositionForPenaltyP.finalSlope ;
      sParam.GoToPointP.finalVelocity   = tParam.PositionForPenaltyP.finalVelocity ;
      sParam.GoToPointP.bot_point_dist_penalty = SkillSet::Bot_Point_dis_penalty_l1;
      sParam.GoToPointP.angle_diff_penalty     = SkillSet::angle_penalty_l1;
      // Execute the selected skill
      skillSet->executeSkill(sID, sParam);
      if (state->homeVel[botID].absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      {
        tState = COMPLETED;
      }
    }
  }; // class TPositionForPenalty
} // namespace MyStrategy
#endif // TPOSITIONFORPENALTY_HPP
