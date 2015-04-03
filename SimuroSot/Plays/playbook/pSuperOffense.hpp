#ifndef PSUPER_OFFENSE_PLAY_HPP
#define PSUPER_OFFENSE_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

#define N_BOT_DANGER 3
#define N_GOAL_DANGER 2

namespace MyStrategy
{
  class PSuperOffensePlay : public Play
  {
    public: 
    inline PSuperOffensePlay(const BeliefState& state) : Play(state)
    {
      name = "SuperOffense";
      assert(HomeTeam::SIZE ==5);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 2 - Striker */
      roleList[1].push_back(std::make_pair(Tactic::Attack,param));
      
      /* Role 3 - Defender */
      param.BlockP.dist = 1000;
      if(state.pr_ball_in_opp_dbox)
        roleList[2].push_back(std::make_pair(Tactic::ChargeBall,param));
      else
        roleList[2].push_back(std::make_pair(Tactic::Block,param));
      
      /* Role 4 - Forward Support Player 1 */
      roleList[3].push_back(std::make_pair(Tactic::ReceiveBall,param));
      
      /* Role 5 - Forward Support Player 2*/
      roleList[4].push_back(std::make_pair(Tactic::Backup,param));
      
      computeMaxTacticTransits();
    }
    
    inline ~PSuperOffensePlay()
    {
      
    }
    
    inline bool applicable(void) const
    {
//      int diff = state.oppGoalCount - state.ourGoalCount;
//      if(diff > N_GOAL_DANGER && state.pr_ballOppSide)
//        return true;
      //printf("Half Field MAXX : %d Should be x > %d actual : %d\n",HALF_FIELD_MAXX,ForwardX(HALF_FIELD_MAXX - DBOX_WIDTH),(int)state.ballPos.x);
      if(!state.pr_gameRunning)
        return false;
      if(state.pr_ball_in_opp_dbox)
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(!state.pr_ball_in_opp_dbox) return FAILURE_LOW;
      return NOT_TERMINATED;
    }
  };
}
#endif // PSUPER_OFFENSE_PLAY_HPP