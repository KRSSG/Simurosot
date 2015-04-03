#ifndef POPPGOALKICK_HPP
#define POPPGOALKICK_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"
using namespace std;

namespace MyStrategy
{
  class POppGoalKick : public Play
  {
    public: 
    inline POppGoalKick(const BeliefState& state) : Play(state)
    {
      name = "OppGoalKick";
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_NO;
      assert(HomeTeam::SIZE ==5);
      Tactic::Param param;
      
      /* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
      
      /* Role 2 -Defender 2*/
      param.DefendLineP.x1 = ForwardX(-HALF_FIELD_MAXX/2);
      param.DefendLineP.y1 = -HALF_FIELD_MAXY;
      param.DefendLineP.x2 = ForwardX(-HALF_FIELD_MAXX/2);
      param.DefendLineP.y2 = HALF_FIELD_MAXY;
      roleList[2].push_back(std::make_pair(Tactic::DefendLine,param));
      
      /* Role 3 - Bot1 ready to intercept ball */
      param.PositionP.x=ForwardX(-HALF_FIELD_MAXX/4);
      param.PositionP.y= -HALF_FIELD_MAXY/2;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[3].push_back(std::make_pair(Tactic::Position,param));

      /* Role 4 - Midfield Player */
      param.PositionP.x=ForwardX(-HALF_FIELD_MAXX/4);
      param.PositionP.y= 0;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
          
    inline ~POppGoalKick()
    {
      
    }
    inline bool applicable(void) const
    {
      
      if(state.pr_oppGoalKick)
        return true;
      return false;
    }
    inline Result done(void) const
    {

        return NOT_TERMINATED;
    }
  };
}
#endif // POPP_GOAL_KICK_HPP