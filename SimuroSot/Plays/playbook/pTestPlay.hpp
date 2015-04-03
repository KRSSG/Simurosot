#ifndef PTEST_PLAY_HPP
#define PTEST_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

namespace MyStrategy
{
  class PTestPlay : public Play
  {
  public:
    inline PTestPlay(const BeliefState& state) 
      : Play(state)
    {
      name = "TestPlay";

      assert(HomeTeam::SIZE == 5); 
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      Tactic::Param pAttack;
      pAttack.AttackP.rotateOnError = true;
      
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::Attack,pAttack));
      
      /* Role 2 - Striker */
      roleList[1].push_back(std::make_pair(Tactic::Stop,param));
      
      /* Role 3 - Defender */
      roleList[2].push_back(std::make_pair(Tactic::Stop,param));
      
      /* Role 4 - Mid Field Player/Charger */
      param.BlockP.dist = 1000;
      if(state.pr_ball_in_opp_dbox)
        roleList[3].push_back(std::make_pair(Tactic::Stop,param));
      else
        roleList[3].push_back(std::make_pair(Tactic::Stop,param));
      
      /* Role 5 - Support Player */
      roleList[4].push_back(std::make_pair(Tactic::Stop,param));
      computeMaxTacticTransits();
    }

    inline ~PTestPlay()
    { }

    inline bool applicable(void) const
    {
      // TODO make it more sophisticated
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      return NOT_TERMINATED;
    }
  }; // class PTestPlay
} // namespace Strategy

#endif // PTEST_PLAY_HPP
