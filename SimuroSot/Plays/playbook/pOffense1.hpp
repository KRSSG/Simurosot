#ifndef POFFENSE1_PLAY_HPP
#define POFFENSE1_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

namespace MyStrategy
{
  class POffense1Play : public Play
  {
    public: 
    inline POffense1Play(const BeliefState& state) : Play(state)
    {
      name = "Offense1";
      assert(HomeTeam::SIZE ==5);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      Tactic::Param pAttack;
      pAttack.AttackP.rotateOnError = true;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));

	   /* Role 2 - Defender */
      roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
      
      /* Role 3 - Mid Field Player/Charger */
	  roleList[2].push_back(std::make_pair(Tactic::MidField,param));

      /* Role 4 - Striker */
      roleList[3].push_back(std::make_pair(Tactic::Attack,pAttack));

	  /* Role 5 - Support Player */
	  roleList[4].push_back(std::make_pair(Tactic::ReceiveBall,param));

		
        
      computeMaxTacticTransits();
    }
    
    inline ~POffense1Play()
    {
      
    }
    
    inline bool applicable(void) const
    {
      
//      if(!state.pr_nOpponentsOurSide(1) && state.pr_ballOurSide)
//        return true;
//      if(state.pr_ballOppSide)
//        return true;
      if(!state.pr_gameRunning)
        return false;
      if(state.pr_ballOppSide)// && !state.pr_ball_in_opp_dbox)
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_ballOurSide ) return FAILURE_LOW;
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      return NOT_TERMINATED;
    }
  };
}
#endif // POFFENSE1_PLAY_HPP