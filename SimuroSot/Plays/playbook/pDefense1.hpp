#ifndef PDEFENSE1_PLAY_HPP
#define PDEFENSE1_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

namespace MyStrategy
{
  class PDefense1Play : public Play
  {
    public: 
    inline PDefense1Play(const BeliefState& state) : Play(state)
    {
      name = "Defense1";
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
      param.DefendPointP.x = ForwardX(HALF_FIELD_MAXX)/2;
      param.DefendPointP.y = HALF_FIELD_MAXY;
      param.DefendPointP.radius = HALF_FIELD_MAXX;
      roleList[3].push_back(std::make_pair(Tactic::DefendPoint,param));
      
      /* Role 5 - Defender 3 */
      param.DefendPointP.x = ForwardX(HALF_FIELD_MAXX)/2;
      param.DefendPointP.y = -HALF_FIELD_MAXY;
      param.DefendPointP.radius = HALF_FIELD_MAXX;
      roleList[4].push_back(std::make_pair(Tactic::DefendPoint,param));
      
      computeMaxTacticTransits();
    }
  
    inline ~PDefense1Play()
    {
      
    }
    
    inline bool applicable(void) const
    {
//      if(state.pr_nOpponentsOurSide(1) &&  && !state.pr_nOpponentsOurSide(3))
//        return true;
      if(!state.pr_gameRunning)
        return false;
      if(state.pr_ballOurSide)// && state.pr_ball_in_our_dbox) 
        return true;
      return false;
    }
    
    inline Result done(void) const
    {
      if(state.pr_ballOppSide) return SUCCESS_LOW;
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ourBall && state.pr_ballOppSide) return SUCCESS_LOW;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      if(state.pr_ball_in_our_dbox) return FAILURE_MED;
      return NOT_TERMINATED;
    }
  };
}
#endif // PDEFENSE1_PLAY_HPP