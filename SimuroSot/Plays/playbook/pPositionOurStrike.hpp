#ifndef PPOSITIONOURSTRIKE_HPP
#define PPOSITIONOURSTRIKE_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

/* This play positions the bots in the position in case we win the toss and We strike the ball first. */
namespace MyStrategy
{
  class PPositionOurStrike : public Play
  {
  public:
    inline PPositionOurStrike(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOurStrike";

      assert(HomeTeam::SIZE == 5); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-2*HALF_FIELD_MAXX)/3;
      param.PositionP.y = 0;
      param.PositionP.finalSlope=0;
      roleList[0].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX)/4;
      param.PositionP.y = HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
      roleList[1].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX)/4;
      param.PositionP.y =-HALF_FIELD_MAXY/2;
      param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-CENTER_CIRCLE_DIAMETER)/2;
      param.PositionP.y = 0;
      param.PositionP.finalSlope  = 0;
      roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
      /* Always make one robot as goalie*/
      param.PositionP.align = false;
      param.PositionP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*1.5);
      param.PositionP.y = 0;
      param.PositionP.finalSlope  = 0;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
        
      computeMaxTacticTransits();
    }

    inline ~PPositionOurStrike()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_ourKickOff)
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

#endif // PTEST_PLAY_HPP
