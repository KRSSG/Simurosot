#ifndef PPOSITIONOURFREEKICK_HPP
#define PPOSITIONOURFREEKICK_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"
#define KICK_RADIUS 200
/* This play positions the bots in the position in case its our Free Kick*/
namespace MyStrategy
{
  class PPositionOurFreeKick : public Play
  {
  public:
    inline PPositionOurFreeKick(const BeliefState& state) 
      : Play(state)
    {
      name = "OurFreeKick";

      assert(HomeTeam::SIZE == 5); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      param.PositionP.align = false;
      param.PositionP.x = state.ballPos.x-ForwardX(KICK_RADIUS+BOT_RADIUS);
      param.PositionP.y = -state.ballPos.y;
      param.PositionP.finalSlope  = PI/2;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x = state.ballPos.x;
      param.PositionP.y = -state.ballPos.y;
      param.PositionP.finalSlope  = 0;
      roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x = (state.ballPos.x+ForwardX(HALF_FIELD_MAXX) )/2;
      param.PositionP.y = (state.ballPos.y)/2;
      param.PositionP.finalSlope  = 0;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }

    inline ~PPositionOurFreeKick()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_ourFreeKick)
        return true;
      return false;
    }

    inline Result done(void) const
    {
      // TODO make it more sophisticated and also use the timeout info to determine if the play has terminated
      // printf("Done condition not finalised\n");
      return NOT_TERMINATED;
    }
  }; // class PTestPlay
} // namespace Strategy

#endif // PPOSITIONOURFREEKICK
