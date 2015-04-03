#ifndef TCHARGETOGOAL_HPP
#define TCHARGETOGOAL_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
#include "../winDebugger/Client.h"

namespace MyStrategy
{
  class TChargeToGoal : public Tactic
  {
  public:
    TChargeToGoal(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TChargeToGoal

    ~TChargeToGoal()
    { } // ~TChargeToGoal

    inline bool isActiveTactic(void) const
    {
      return false;
    }
	bool isBotAlignedWithBall(int _botID) const
	{
	float finalSlope = atan2(state->ballPos.y-state->homePos[_botID].y,state->ballPos.x-state->homePos[_botID].x);
	float angleWithBall = normalizeAngle(finalSlope - state->homeAngle[_botID]); 
	if(angleWithBall>PI/2||angleWithBall<-PI/2)
	{
		if(angleWithBall>PI/2)
		angleWithBall=angleWithBall-PI;
		else
		angleWithBall=angleWithBall+PI;
	}
	char debug[250];
	sprintf(debug,"Bot %d anglewithball = %f\n",_botID,fabs(angleWithBall));
	////Client::debugClient->SendMessages(debug);
	if(fabs(angleWithBall) < DRIBBLER_BALL_ANGLE_RANGE) 
		return true;
	return false;
	}
	bool isBotBallGoalInLine( int _botID) const
	{
	int goalX = HALF_FIELD_MAXX;
	int goalY ;
	float slope = (1.0*(state->ballPos.y-state->homePos[_botID].y))/(state->ballPos.x-state->homePos[_botID].x);
	goalY = slope*(goalX - state->ballPos.x) + state->ballPos.y;
	char debug[250];
	sprintf(debug,"Bot %d Goal Y = %d\n",_botID,goalY);
	////Client::debugClient->SendMessages(debug);
	if(goalY > OPP_GOAL_MINY && goalY < OPP_GOAL_MAXY)
		return true;
	return false;
	}
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam)  const
    {
		////Client::debugClient->SendMessages("Choose best bot \n,,,,,,,,,,,,,,,,,,,,");
	  float minDistFromBall = 10000.0;
	  int bestBot = *(freeBots.begin()) ;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
		 // isBotBallGoalInLine(2);
		if(isBotAlignedWithBall(*it) && isBotBallGoalInLine(*it))
		{
			float dis_from_ball = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
			if(dis_from_ball < minDistFromBall)
			{
				minDistFromBall = dis_from_ball;
				bestBot = *it;
			}
		}
      }
	  char debug[250];
	sprintf(debug,"Best Bot id = %d \n",bestBot);
	////Client::debugClient->SendMessages(debug);
	
	  return bestBot;
     // Util::// Loggerabort("No bot available for role assignment");
    } // chooseBestBot

    void execute(const Param& tParam)
    {
		  char debug[250];
	sprintf(debug,"Best Bot id = %d \n",botID);
	//Client::debugClient->SendMessages(debug);
		sID = SkillSet::ChargeBall;
		isBotBallGoalInLine(botID);
		skillSet->executeSkill(sID, sParam);
    }
  }; // class TChargeToGoal
} // namespace MyStrategy

#endif // TCHARGETOGOAL_HPP
