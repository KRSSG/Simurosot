#ifndef TTATTACKLINGO_HPP
#define TTATTACKLINGO_HPP

#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../common/include/geometry.hpp"
#include "../Utils/intersection.hpp"
#include "../winDebugger/Client.h"
#include "../Utils/comdef.h"
#include "../common/include/config.h"


#include "../HAL/comm.h"


namespace MyStrategy
{
  class TAttackLingo : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TAttackLingo(const BeliefState* state, int botID) :
      Tactic(Tactic::AttackLingo, state, botID)
    {
		iState=ATTACK_LINGO;
      for(int i=0; i<10; i++)
      movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      hasAchievedOffset = 0;
    } // TAttack

    ~TAttackLingo()
    { } // ~TAttack
    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }
	 enum InternalState
    {
      ATTACK_LINGO,
      DEFEND,
      FORM_SWARM
    } iState;

    bool pointxInField(Vector2D<int> final)
    {
      if((final.x < HALF_FIELD_MAXX - (BALL_AT_CORNER_THRESH) && final.x > -HALF_FIELD_MAXX + (BALL_AT_CORNER_THRESH)))
      {
        if((final.y < HALF_FIELD_MAXY - BALL_AT_CORNER_THRESH && final.y > -HALF_FIELD_MAXY + BALL_AT_CORNER_THRESH))
        {
          return true;
        }
        else return false;
      }
      else  return false;
    }
	
int opponentProbableGoalkeeper(){
int distance=HALF_FIELD_MAXX/2;
int id=-1;
for(int i=0; i <5 ; i++){
if(HALF_FIELD_MAXX-ForwardX(state->awayPos[i].x) < distance){
if(state->awayPos[i].y < OPP_GOAL_MAXY + 2*BOT_RADIUS && state->awayPos[i].y > OPP_GOAL_MINY -2*BOT_RADIUS){ 
     distance=HALF_FIELD_MAXX-ForwardX(state->awayPos[i].x);
 id=i;
}
}
} 
return id;
}


	
  void execute(const Param& tParam)
    { 

		switch(iState)
		{
			//*******************************************  gunjan ******************************************
			case ATTACK_LINGO :
			{
				if(abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[state->ourBotNearestToBall].y)>HALF_FIELD_MAXY-2*BOT_RADIUS && (state->homePos[botID].x>state->homePos[state->ourBotNearestToBall].x  ))
				{
					iState=FORM_SWARM;
					break;
				}
				    	  float offset = 600;
						  float factor = 0.00005;
		  						  int attID=state->ourBotNearestToBall;
						  float ang=Vector2D<int>::angle(state->ballPos, state->homePos[botID]); //attID;  //changed ..............................
						  float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
						  int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
						  int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x - BOT_RADIUS*cos(ang);//1
						  int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y - BOT_RADIUS*sin(ang);  //1

						  int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist - BOT_RADIUS*cos(ang);
						  int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist - BOT_RADIUS*sin(ang);

					  sID = SkillSet::GoToPoint;
  
					  if(targetX < (-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS))   //changed acc to rules
					  {
						  targetX = -HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS;
					  }
	  
					  if((targetX >( HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS))&&(abs(targetY) < OUR_GOAL_MAXY + BOT_RADIUS))  //changed acc to rules.................
						  {
							  targetY = SGN(targetY)*(OUR_GOAL_MAXY + BOT_RADIUS) ;
							  targetX =  HALF_FIELD_MAXX -GOAL_DEPTH - 3*BOT_RADIUS ;
						  } 
	    
						sParam.GoToPointP.x = targetX;
						sParam.GoToPointP.y = targetY;
					  sParam.GoToPointP.align = true;
					  sParam.GoToPointP.increaseSpeed = true;
					   skillSet->executeSkill(sID, sParam);
			}

			case FORM_SWARM :
				{
					Vector2D<int> dest;
					if(state->homePos[botID].x > state->homePos[state->ourBotNearestToBall].x && abs(state->ballPos.y) > HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[botID].y)<HALF_FIELD_MAXY-2*BOT_RADIUS)
					{
						dest.x=state->homePos[state->ourBotNearestToBall].x-3*BOT_RADIUS;
						dest.y=state->homePos[state->ourBotNearestToBall].y-SGN(state->homePos[state->ourBotNearestToBall].y)*2*BOT_RADIUS;
					}
					else if(state->homePos[botID].x > state->homePos[state->ourBotNearestToBall].x && abs(state->ballPos.y) > HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[botID].y)>HALF_FIELD_MAXY-2*BOT_RADIUS)
					{
						dest.y=state->homePos[state->ourBotNearestToBall].y-SGN(state->homePos[state->ourBotNearestToBall].y)*2.5*BOT_RADIUS;
						dest.x=state->homePos[botID].x;
					}
					
					else
					{
						iState=ATTACK_LINGO;
						break;
					}
					  sID = SkillSet::GoToPoint;
					  sParam.GoToPointP.x = dest.x;
				      sParam.GoToPointP.y = dest.y;
					  sParam.GoToPointP.align = true;
					  sParam.GoToPointP.increaseSpeed = true;
					  sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;
					   skillSet->executeSkill(sID, sParam);


				}
			 
				
		}// switch 

    }// execute
  
	bool isBallInDBox()
  {
      if((abs(state->ballPos.y)<0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < -HALF_FIELD_MAXX*0.7)) 
          return true;

		  return false;

  
  }
    };//class TAttack
} // namespace MyStrategy
#endif // TTCharge_HPP