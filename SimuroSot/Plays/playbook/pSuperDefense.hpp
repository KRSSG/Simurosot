#ifndef PSUPER_DEFENSE_PLAY_HPP
#define PSUPER_DEFENSE_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

namespace MyStrategy
{
  class PSuperDefensePlay : public Play
  {
    public: 
    inline PSuperDefensePlay(const BeliefState& state) : Play(state)
    {
      name = "SuperDefense";
      assert(HomeTeam::SIZE ==5);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 2 - Striker */
      roleList[1].push_back(std::make_pair(Tactic::Attack,param));
      
      /* Role 3 - Defender 1 */
      roleList[2].push_back(std::make_pair(Tactic::CoverGoal,param));
      
      /* Role 4 - Defender 2 */
      param.DefendLineP.x1 = ForwardX(-HALF_FIELD_MAXX)/2;
      param.DefendLineP.x2 = ForwardX(-HALF_FIELD_MAXX)/2;
      param.DefendLineP.y1 = 0;
      param.DefendLineP.y2 = HALF_FIELD_MAXY - 2*BOT_RADIUS;
      roleList[3].push_back(std::make_pair(Tactic::DefendLine,param));
      
      /* Role 5 - Defender 3 */
      param.DefendLineP.x1 = ForwardX(-HALF_FIELD_MAXX)/2;
      param.DefendLineP.x2 = ForwardX(-HALF_FIELD_MAXX)/2;
      param.DefendLineP.y1 = 0;
      param.DefendLineP.y2 = -HALF_FIELD_MAXY + 2*BOT_RADIUS;
      roleList[4].push_back(std::make_pair(Tactic::DefendLine,param));
	  
      computeMaxTacticTransits();
    }
    
    inline ~PSuperDefensePlay()
    {
      
    }
    
    inline bool applicable(void) const
    {
//      int diff = state.oppGoalCount - state.ourGoalCount;
//      if((state.pr_nOpponentsOurSide(3) && state.pr_ballOurSide) || 
//              (diff >= 3 && diff <= 5 && state.pr_ballOurSide) || (diff <= -3 && state.pr_ballOurSide))
//        return true;
      if(!state.pr_gameRunning)
        return false;
      if(state.pr_ball_in_our_dbox)
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ourBall && state.pr_ballOppSide) return SUCCESS_LOW;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      if(!state.pr_ball_in_our_dbox) return SUCCESS_LOW;
      return NOT_TERMINATED;
    }
  };
}
#endif // PSUPER_DEFENSE_PLAY_HPP