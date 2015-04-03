#ifndef POFFENSIVE3_PLAY_HPP
#define POFFENSIVE3_PLAY_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"
#include "../../winDebugger/Client.h"



namespace MyStrategy
{
  class POffense3Play : public Play
  {
    public: 
    inline POffense3Play(const BeliefState& state) : Play(state)
    {
      name = "Offense3";
      assert(HomeTeam::SIZE ==5);
      
      PositionPlay = PLAYTYPE_NO;
      AttackPlay   = PLAYTYPE_YES;
      Tactic::Param param;
	  ////Client::debugClient->SendMessages("Play Started\n");
      /* Role 1 - Goalie */
      roleList[0].push_back(std::make_pair(Tactic::GoalieOur,param));
      
      /* Role 2 - Striker */
	  if(applicable())
	  {
		  //Client::debugClient->SendMessages("ChargeToGoal....................\n");
		  roleList[1].push_back(std::make_pair(Tactic::ChargeToGoal,param));
		  roleList[2].push_back(std::make_pair(Tactic::Attack,param));
	  }
		
	  else
	  {
		  roleList[1].push_back(std::make_pair(Tactic::Attack,param));
      /* Role 3 - Defender */
		  roleList[2].push_back(std::make_pair(Tactic::Block,param));
	  }
      
      
      /* Role 4 - Mid Field Player/Charger */
      param.BlockP.dist = 1700;
      //if(state.pr_ball_in_opp_dbox  && !state.pr_ourBall)
        //roleList[3].push_back(std::make_pair(Tactic::ChargeBall,param));
      //else
	  roleList[3].push_back(std::make_pair(Tactic::CoverGoal,param));
      
      /* Role 5 - Support Player */
	  roleList[4].push_back(std::make_pair(Tactic::ReceiveBall,param));  
	  ////Client::debugClient->SendMessages("computeMaxTacticTransits\n");
      computeMaxTacticTransits();
    }
    
    inline ~POffense3Play()
    {
      
    }
    
    inline bool applicable(void) const
    {
//      if(!state.pr_nOpponentsOurSide(1) && state.pr_ballOurSide)
//        return true;
//      if(state.pr_ballOppSide)
//        return true;
		char data[250];
    	sprintf(data,"Inside applicable\n");
		////Client::debugClient->SendMessages(data);
		for(int i=0;i<HomeTeam::SIZE;i++)
		{
			if(isBotAlignedWithBall(i) && isBotBallGoalInLine(i))
				return true;
		}
		return false;
    }
    bool isBotAlignedWithBall(int _botID) const
	{
	float finalSlope = atan2(state.ballPos.y-state.homePos[_botID].y,state.ballPos.x-state.homePos[_botID].x);
	float angleWithBall = normalizeAngle(finalSlope - state.homeAngle[_botID]); 
	if(angleWithBall>PI/2||angleWithBall<-PI/2)
	{
		if(angleWithBall>PI/2)
		angleWithBall=angleWithBall-PI;
		else
		angleWithBall=angleWithBall+PI;
	} 
	if(fabs(angleWithBall) < DRIBBLER_BALL_ANGLE_RANGE/2) 
		return true;
	return false;
	}
	bool isBotBallGoalInLine( int _botID) const
	{
	int goalX = HALF_FIELD_MAXX;
	int goalY ;
	float slope = (1.0*(state.ballPos.y-state.homePos[_botID].y))/(state.ballPos.x-state.homePos[_botID].x);
	goalY = slope*(goalX - state.ballPos.x) + state.ballPos.y;
	/*char debug[250];
	sprintf(debug,"Expected Goal Y = %d\n",goalY);
	//Client::debugClient.SendMessages(debug);*/
	if(goalY > OPP_GOAL_MINY && goalY < OPP_GOAL_MAXY)
		return true;
	return false;
	}
    inline Result done(void) const
    {
      if(state.pr_ballOurSide) return FAILURE_LOW;
      if(state.pr_oppGoalScored) return FAILURE_HIGH;
      if(state.pr_ballOurSide && state.pr_oppBall) return FAILURE_MED;
      if(state.pr_ourGoalScored) return SUCCESS_HIGH;
      if(state.pr_ball_in_opp_dbox) return SUCCESS_MED;
      return NOT_TERMINATED;
    }
  };
}
#endif // POFFENSIVE3_PLAY_HPP