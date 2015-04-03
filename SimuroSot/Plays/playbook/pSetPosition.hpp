#ifndef PSET_POSITION_HPP
#define PSET_POSITION_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

#define GAP SELECT(1000,100)

namespace MyStrategy
{
  class PSetPosition : public Play
  {
  public:
    inline PSetPosition(const BeliefState& state) 
      : Play(state)
    {
      name = "SetPositon";

      assert(HomeTeam::SIZE == 5); // TestPlay is applicable for a team of 3 robots only

      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      Tactic::Param param;
      //for (int botIDx=0; botIDx<HomeTeam::SIZE; ++botIDx)     
      param.PositionP.x=CENTER_X;
      param.PositionP.y=CENTER_Y;
      param.PositionP.finalSlope=0;
      param.PositionP.align = false;
      roleList[0].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x= CENTER_X;
      param.PositionP.y= CENTER_Y + GAP;
      param.PositionP.finalSlope=-PI/2;
      roleList[1].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x= CENTER_X;
      param.PositionP.y= CENTER_Y - GAP;
      param.PositionP.finalSlope= PI/2;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x= CENTER_X - GAP/2;
      param.PositionP.y= CENTER_Y + GAP/2;
      param.PositionP.finalSlope= -PI/4;
      roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
      param.PositionP.x= CENTER_X - GAP/2;
      param.PositionP.y= CENTER_Y - GAP/2;
      param.PositionP.finalSlope= PI/4;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }

    inline ~PSetPosition()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_oppPenaltyKick)
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
