#ifndef PPOSITIONOPPFREEKICK_HPP
#define PPOSITIONOPPFREEKICK_HPP


#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"
#define FREEKICK_RADIUS HALF_FIELD_MAXX/4
/* This play positions the bots in the position in case we Defend a Free Kick*/
namespace MyStrategy
{
  class PPositionOppFreeKick : public Play
  {
  public:
    inline PPositionOppFreeKick(const BeliefState& state) 
      : Play(state)
    {
      name = "PositionOppFreeKick";

      assert(HomeTeam::SIZE == 5); 
      
      PositionPlay = PLAYTYPE_YES;
      AttackPlay   = PLAYTYPE_NO;
      
      Tactic::Param param;
	Vector2D<int> finalPoint;
    //  roleList[1].push_back(std::make_pair(Tactic::Position, param));
			
			/* Role 0 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 1 - Defender 1*/
      
	  roleList[1].push_back(std::make_pair(Tactic::CoverGoal,param));
      
	  //Role 2 
	  Vector2D<int> dpoint(state.ballPos.x,-1*state.ballPos.y);
	//  Client::debugClient->SendMessages("going");
	 // finalPoint = position_correcter(&state,dpoint);
	 finalPoint.x = dpoint.x ; finalPoint.y = dpoint.y ;

	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  param.PositionP.finalSlope  = 0;
      roleList[2].push_back(std::make_pair(Tactic::Position, param));
      
	  //Role 3
	  Vector2D<int> dpoint1(state.ballPos.x-ForwardX(FREEKICK_RADIUS),state.ballPos.y);
	  //finalPoint = position_correcter(&state,dpoint1);
	  //param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
      Vector2D<int>Goal(ForwardX(HALF_FIELD_MAXX),0);
      param.PositionP.finalSlope  = Vector2D<int>::angle(state.ballPos,Goal);
      roleList[3].push_back(std::make_pair(Tactic::Position, param));
      
	  //Role 4
	  Vector2D<int> dpoint2(state.ballPos.x-ForwardX(HALF_FIELD_MAXX)/4,(state.ballPos.y)/2);
	  //finalPoint = position_correcter(&state,dpoint2);
	  finalPoint = dpoint2;
	  param.PositionP.x = finalPoint.x ; param.PositionP.y = finalPoint.y ;
	  param.PositionP.finalSlope  = 0;
      roleList[4].push_back(std::make_pair(Tactic::Position, param));
      
      computeMaxTacticTransits();
    }
	/*
	virtual void reevaluateRoleParams()
    {
        {
            Tactic::Param &param = roleList[4][0].second;
            param.PositionP.x = (state.ballPos.x+ForwardX(HALF_FIELD_MAXX))/2;
						param.PositionP.y = (state.ballPos.y)/2;
						param.PositionP.finalSlope  = 0;
        }
        {
            Tactic::Param &param = roleList[3][0].second;
            param.PositionP.x = state.ballPos.x-ForwardX(FREEKICK_RADIUS);
						param.PositionP.y = state.ballPos.y;
						param.PositionP.finalSlope  = 0;
        }
        {
            Tactic::Param &param = roleList[2][0].second;
            param.PositionP.x = state.ballPos.x;
						param.PositionP.y = SGN(-state.ballPos.y)*HALF_FIELD_MAXY/2;;
						param.PositionP.finalSlope  = 0;
        }
        printf("reevaling params in OurFreeKick. ballPos = %d, %d\n", state.ballPos.x, state.ballPos.y);
    }
	*/
    inline ~PPositionOppFreeKick()
    { }

    inline bool applicable(void) const
    {
      if(state.pr_oppFreeKick)
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

#endif // PPOSITIONOPPFREEKICK