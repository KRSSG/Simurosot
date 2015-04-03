#ifndef POURGOALKICK_HPP
#define POURGOALKICK_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"
using namespace std;

namespace MyStrategy
{
  class POurGoalKick : public Play
  {
    public: 
    inline POurGoalKick(const BeliefState& state) : Play(state)
    {
      name = "OurGoalKick";
      assert(HomeTeam::SIZE ==5);
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
      
      /* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - in our half position1*/
      param.PositionP.x=ForwardX(-HALF_FIELD_MAXX/4);
      param.PositionP.y= HALF_FIELD_MAXY/4;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[1].push_back(std::make_pair(Tactic::DefendLine,param));
      
      /* Role 2 -in our half position 2 */
      param.PositionP.x=ForwardX(-HALF_FIELD_MAXX/4);
      param.PositionP.y= -HALF_FIELD_MAXY/4;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[2].push_back(std::make_pair(Tactic::DefendLine,param));
      
      /* Role 3 - Bot1 ready to recieve ball in opp half*/
      param.PositionP.x=ForwardX(2*HALF_FIELD_MAXX/3);
      param.PositionP.y= -HALF_FIELD_MAXY/2;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[3].push_back(std::make_pair(Tactic::Position,param));
      
      /* Role 4 - Midfield Player */
      param.PositionP.x=ForwardX(2*HALF_FIELD_MAXX/3);
      param.PositionP.y= HALF_FIELD_MAXY/2;;
      param.PositionP.align=true;
      param.PositionP.finalSlope= PI/2;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
          
    inline ~POurGoalKick()
    {
      
    }
    inline bool applicable(void) const
    {
      if(state.pr_ourGoalKick)
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
