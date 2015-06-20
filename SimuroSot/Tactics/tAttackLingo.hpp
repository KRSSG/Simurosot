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
		//*******************************************  gunjan ******************************************
		float d=Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
		
		
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
		 
		 float angle =Vector2D<int> ::angle(state->ballPos,Vector2D<int>(x3,y3));
		 float angle1 =Vector2D<int> ::angle(state->homePos[botID],Vector2D<int>(x3,y3));

		 float d1=Vector2D<int>::dist(state->homePos[botID],Vector2D<int>(x3,y3));

		switch(iState)
		{

			case ATTACK_LINGO :
			{
				if(Vector2D<int>::dist(state->ballPos,state->homePos[state->oppBotNearestToBall])<2*BOT_BALL_THRESH && state->ballPos.x>-HALF_FIELD_MAXX+GOAL_DEPTH+3.5*BOT_RADIUS && abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[state->ourBotNearestToBall].y)>HALF_FIELD_MAXY-2*BOT_RADIUS && (state->homePos[botID].x>state->homePos[state->ourBotNearestToBall].x ))
				{
					iState=FORM_SWARM;
					break;
				}
				/*else if(state->ballPos.x<0 && state->ballVel.x<0   && state->ballPos.x>state->homePos[botID].x && state->ballPos.x>(-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + 2*BOT_RADIUS))
				{
					iState=DEFEND;
					break;

				}*/
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
					  if( state->ballPos.x < (-HALF_FIELD_MAXX + GOAL_DEPTH +2*BOT_RADIUS) && abs(state->ballPos.y) >HALF_FIELD_MAXY*0.6)
					  {
						 
						  if(state->ballPos.y<0&& state->ballPos.y<state->homePos[botID].y)
						  {
							   targetX=state->ballPos.x;
							  targetY=state->ballPos.y;
						  }
						  if(state->ballPos.y>0&& state->ballPos.y>state->homePos[botID].y)
						  {
							   targetX=state->ballPos.x;
							  targetY=state->ballPos.y;
						  }
						  sParam.GoToPointP.finalslope =PI/2;
					  }
  
					 
	  
					  if((targetX >( HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS))&&(abs(targetY) < OUR_GOAL_MAXY + BOT_RADIUS))  //changed acc to rules.................
						  {
							  targetY = SGN(targetY)*(OUR_GOAL_MAXY + BOT_RADIUS) ;
							  targetX =  HALF_FIELD_MAXX -GOAL_DEPTH - 2*BOT_RADIUS ;
						  } 
					  if(state->ballPos.x>HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS*2 &&state->homePos[botID].x>HALF_FIELD_MAXX-GOAL_DEPTH-2*BOT_RADIUS && ((state->homePos[botID].y>state->ballPos.y && state->ballPos.y<0)||(state->homePos[botID].y<state->ballPos.y && state->ballPos.y>0)))
					  {
						  if (state->homePos[botID].y>state->ballPos.y && state->ballPos.y<0)
						  {
							  targetX=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH-4*BOT_RADIUS;
							  targetY=OPP_GOAL_MINY-BOT_RADIUS;
						  }
						  else if(state->homePos[botID].y<state->ballPos.y && state->ballPos.y>0)
						  {
							  targetX=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH-1*BOT_RADIUS;
							  targetY=OPP_GOAL_MAXY+BOT_RADIUS;
						  }
					  }
					
						
						
						if(targetX < (-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS)&& abs(state->ballPos.y) <HALF_FIELD_MAXY*0.6)   //changed acc to rules
					  {
						  targetX = -HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS;
						  targetY=-1*SGN(state->ballPos.y)*state->ballPos.y;
					  }
						sParam.GoToPointP.x = targetX;
						sParam.GoToPointP.y = targetY;

					  if(state->ballPos.x<-HALF_FIELD_MAXX+GOAL_DEPTH+2*BOT_RADIUS && abs(state->ballPos.y)<OUR_GOAL_MAXY+4*BOT_RADIUS)      // acc to rules....................
						{
							sID=SkillSet::GoToPoint;
							sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH +DBOX_WIDTH+BOT_RADIUS;
							if(state->ballPos.y >0)
							  sParam.GoToPointP.y =-(OUR_GOAL_MAXY+2*BOT_RADIUS);
							else
							  sParam.GoToPointP.y =(OUR_GOAL_MAXY+2*BOT_RADIUS);
					
					
						}	

						//////////////////////////////////////////////////////


						if (state->homePos[0].x + 0.3*BOT_RADIUS > state->homePos[botID].x)		//Defender will not disturb GoalKeeper from the back: By KD
						{
							if (state->ballPos.y > 0)
								{
									sID = SkillSet::GoToPoint;
									if (Vector2D<int>::dist(state->homePos[0], state->homePos[botID]) <= 3*BOT_RADIUS)
									{
										sParam.GoToPointP.align = false;
										sParam.GoToPointP.finalslope = PI/2;
										sParam.GoToPointP.x = state->homePos[botID].x;
										sParam.GoToPointP.y = state->homePos[botID].y - 2*BOT_RADIUS;
									}
									else
									{
										sParam.GoToPointP.align = false;
										sParam.GoToPointP.finalslope = 0;
										sParam.GoToPointP.x = state->homePos[botID].x + 2*BOT_RADIUS;
										sParam.GoToPointP.y = state->homePos[botID].y;
									}
								}
								else
								{
									sID = SkillSet::GoToPoint;
									if (Vector2D<int>::dist(state->homePos[0], state->homePos[botID]) <= 3.5*BOT_RADIUS)
									{
										sParam.GoToPointP.align = false;
										sParam.GoToPointP.finalslope = PI/2;
										sParam.GoToPointP.x = state->homePos[botID].x;
										sParam.GoToPointP.y = state->homePos[botID].y + 2*BOT_RADIUS;
									}
									else
									{
										sParam.GoToPointP.align = false;
										sParam.GoToPointP.finalslope = 0;
										sParam.GoToPointP.x = state->homePos[botID].x + 2*BOT_RADIUS;
										sParam.GoToPointP.y = state->homePos[botID].y;
									}
								}
				
						}					//Totally working: Approved. Now it works universally
						



						/////////////////////////////////////////////////////
					  	
						
						sParam.GoToPointP.align = true;
					  sParam.GoToPointP.increaseSpeed = true;
					   skillSet->executeSkill(sID, sParam);
			}

			case FORM_SWARM :
				{
					Vector2D<int> dest;
					if(Vector2D<int>::dist(state->ballPos,state->homePos[state->oppBotNearestToBall])<2*BOT_BALL_THRESH && state->homePos[botID].x > state->homePos[state->ourBotNearestToBall].x && abs(state->ballPos.y) > HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[botID].y)<HALF_FIELD_MAXY-2*BOT_RADIUS)
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
			case DEFEND :
				{
							  if(state->ballPos.x>-HALF_FIELD_MAXX+GOAL_DEPTH+6*BOT_RADIUS && abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS && abs(state->homePos[state->ourBotNearestToBall].y)>HALF_FIELD_MAXY-2*BOT_RADIUS && (state->homePos[botID].x>state->homePos[state->ourBotNearestToBall].x  ))
							{
								iState=FORM_SWARM;
								break;
							}
						
						 
    					if(d<2*BOT_BALL_THRESH && Vector2D<int>::dist(state->homePos[botID],Vector2D<int>(x3,y3))<300 && state->ballPos.x>state->homePos[botID].x)
						{
							sID = SkillSet::GoToPointDW;
							sParam.GoToPointDWP.x = state->ballPos.x;
							sParam.GoToPointDWP.y = state->ballPos.y;

							sParam.GoToPointDWP.finalslope = (state->ballVel.y/state->ballVel.y);
							skillSet->executeSkill(sID, sParam);
							return;
						}

						
						if (state->ballPos.x<0 && state->ballVel.x<0   && state->ballPos.x>state->homePos[botID].x&& state->ballPos.x>(-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS))
						{
		
						
								  sID = SkillSet::DefendPoint;	     
								  sParam.DefendPointP.x = ForwardX(-HALF_FIELD_MAXX +  GOAL_DEPTH);
								  sParam.DefendPointP.y = 0;
								  sParam.DefendPointP.finalslope = Vector2D<int>::angle(state->ballPos,state->homePos[botID]);
								  skillSet->executeSkill(sID, sParam);
						  
						}
						else
					  {
						iState=ATTACK_LINGO;
						break;
					  }
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