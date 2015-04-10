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

#define MIN(a,b) a<b?a:b

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
      iState = APPROACHING;
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
    enum InternalState
    {
      APPROACHING,
      SPINNING_CCW ,
      SPINNING_CW,
      ATTACKING,
      CLOSE_TO_BALL,
      STUCK
    } iState;
    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
		/*
      int minv = *(freeBots.begin());
      float angle_difference = firaNormalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)- state->homeAngle[*(freeBots.begin())]);
      int minwt = Vector2D<int>::dist(state->homePos[*(freeBots.begin())],state->ballPos) + angle_difference * ANGLE_TO_DIST;
      
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = Vector2D<int>::dist(state->homePos[*it],state->ballPos);
        float botballangle = normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[*it]));
        //TODO might require normalization
        float botball_orientation_diff = MIN(fabs((float)botballangle-state->homeAngle[*it]),fabs((float)botballangle-(state->homeAngle[*it]+PI)));
        float finalOrientationDiff = normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos)-botballangle);
//        angle_difference =  fabs((float)firaNormalizeAngle(state->homeAngle[*it]-normalizeAngle(Vector2D<int>::angle(Vector2D<int>(OPP_GOAL_X, 0), state->ballPos))))+ fabs((float)firaNormalizeAngle((Vector2D<int>::angle(state->homePos[*it],Vector2D<int>(OPP_GOAL_X, 0)))));
      angle_difference = botball_orientation_diff + finalOrientationDiff;
  //float x_diff = ForwardX(state->ballPos.x)-ForwardX(state->homePos.x);
        float weight;
    printf("%d >>>>>>>>>> %f , %f\n", *it,dis_from_ball,angle_difference);
        weight = dis_from_ball + ANGLE_TO_DIST * angle_difference;
        //if(*it == botID)
      //    weight -= HYSTERESIS;
        if(weight < minwt)
        {
          minwt = dis_from_ball ;
          minv = *it;
        }
      }
     // Util::// LoggertoStdOut("Selected bot %d\n", minv);
      return minv;
	  */
		return 3 ;
    } // chooseBestBot

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

int getAttacker()
{
	int id = 2 ;
	int dist = 2*HALF_FIELD_MAXX ;
 for(int i = 2 ; i < 4 ; i++)
 {
	 int botballDist = Vector2D<int>::dist(state->ballPos,state->homePos[botID]) ;
	 if(botballDist<dist)
	 {
	  dist = botballDist ;
	  id = i ;
	 }
 }
 return id ;
}
Vector2D<int> decidedGoalPoint_new(){
int id=opponentProbableGoalkeeper();
Vector2D<int> pointToAttack;
 if(id==-1){
	  pointToAttack.x=HALF_FIELD_MAXX-GOAL_DEPTH;
	  pointToAttack.y=0;
	  return pointToAttack;
 }
 Vector2D<int> maxAttackP , minAttackP ;
 Vector2D<int> minGoal(HALF_FIELD_MAXX - GOAL_DEPTH + 0.5*BOT_RADIUS,OPP_GOAL_MINY) ;
 Vector2D<int> maxGoal(HALF_FIELD_MAXX - GOAL_DEPTH + 0.5*BOT_RADIUS,OPP_GOAL_MAXY) ;
 float errAngle = PI*10/180 ;
 
 if(state->ballPos.x < 0.6*HALF_FIELD_MAXX)
 {
      pointToAttack.x=HALF_FIELD_MAXX-GOAL_DEPTH;
	  pointToAttack.y=0;
	  return pointToAttack;
 }
 else
 {
    float maxshift = abs(abs(maxGoal.y-state->ballPos.y) - abs((maxGoal.x - state->ballPos.x)*tan(atan((maxGoal.y-state->ballPos.y)/(maxGoal.x - state->ballPos.x)) + errAngle))) ;
	maxAttackP.x = HALF_FIELD_MAXX - GOAL_DEPTH + 0.5*BOT_RADIUS ;
	maxAttackP.y = maxGoal.y - maxshift ;
	float minshift = abs(abs(minGoal.y-state->ballPos.y) - abs((minGoal.x - state->ballPos.x)*tan(atan((state->ballPos.y - minGoal.x)/(minGoal.x - state->ballPos.x)) - errAngle))) ;
    minAttackP.x = HALF_FIELD_MAXX - GOAL_DEPTH + 0.5*BOT_RADIUS ;
	minAttackP.y = minGoal.y + minshift ;
    
	if(state->ballPos.x < 0.3*HALF_FIELD_MAXX)
	{
	  if(state->awayVel[id].y > 0 )
	  {
	   pointToAttack.x = minAttackP.x ;
	   pointToAttack.y = minAttackP.y ;
	  }
	  else
	  {
	   pointToAttack.x = maxAttackP.x ;
	   pointToAttack.y = maxAttackP.y ;  
	  }
	}
	return pointToAttack ;
 }

}

	
  void execute(const Param& tParam)
    {  float offset = 600;
          float factor = 0.00005;
		  char debug[250] ;
		  		  int attID=getAttacker();//state->ourBotNearestToBall;
		  float ang=Vector2D<int>::angle(state->ballPos, state->homePos[botID]); //attID;  //changed ..............................
		  float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(decidedGoalPoint_new().x,decidedGoalPoint_new().y));
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x - BOT_RADIUS*cos(ang);//1
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y - BOT_RADIUS*sin(ang);  //1

		  int x3 = (targetX * (ballgoaldist + offset) - offset * decidedGoalPoint_new().x) / ballgoaldist - BOT_RADIUS*cos(ang);
          int y3 = (targetY * (ballgoaldist + offset) - offset * decidedGoalPoint_new().y ) / ballgoaldist - BOT_RADIUS*sin(ang);

		  if((state->ballPos.x > state->homePos[botID].x)&&(state->homePos[botID].x > HALF_FIELD_MAXX - GOAL_DEPTH - 0.7*BOT_RADIUS - 7*BOT_RADIUS)&&(abs(state->homePos[botID].y)< OPP_GOAL_MAXY+ BOT_RADIUS))
		  {/*
		       float shootFactor = (state->ballPos.x - state->homePos[botID].x)/(state->homePos[botID].y - state->ballPos.y) ;
				 float ballSpeed = sqrt((int)(state->ballVel.x)^2 + (int)(state->ballVel.y)^2);
				 float phi , theta ; 
				 if(abs(state->ballPos.y) < 10)
					 phi = 0 ;
				 else
					 phi = atan(state->ballPos.x/(-1*state->ballPos.y));

				 theta = state->homeAngle[botID] ;
				 float num = sin(phi) + shootFactor*cos(phi) ;
				 float denum = cos(theta) - shootFactor*sin(theta) ;
				 float v = (num/denum)*ballSpeed ;
				 if(abs(v)>MAX_BOT_SPEED)
					 v = SGN(v)*MAX_BOT_SPEED ;
				  v  = -1*SGN(state->homePos[botID].y)*v ;
				  sID = SkillSet::Velocity;
				  sParam.VelocityP.vl = v ;
				  sParam.VelocityP.vr = v ;
			*/
			  float facto = 5 ;
			  if(ballBotDist<4*BOT_BALL_THRESH)
			  {
			      sID = SkillSet::Velocity;
				  sParam.VelocityP.vl = 90 ; //facto*ballBotDist ;
				  sParam.VelocityP.vr = 90 ; //facto*ballBotDist ;
			  
			     // sprintf(debug,"dragging with v = %f \n",factor*ballBotDist);
                 // Client::debugClient->SendMessages(debug);

				  sprintf(debug,"attacklingo : %d",botID) ;
					 Client::debugClient->SendMessages(debug);
				  skillSet->executeSkill(sID,sParam) ;
		     	   return ;
			  }
		  }
		  if((abs(state->ballPos.y) > 0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < state->homePos[botID].x)&&(ballBotDist<4*BOT_BALL_THRESH))
		  {  
			  sID = SkillSet::GoToPoint ;
			  sParam.GoToPointP.x = state->ballPos.x - abs(state->ballVel.x)*((int)ballBotDist*40*factor);
			  sParam.GoToPointP.y = state->ballPos.y - SGN(state->ballPos.y)*3*BOT_BALL_THRESH ;
              skillSet->executeSkill(sID,sParam) ;
		      return;
		  }


	
      sID = SkillSet::GoToPoint;
/*	if(isBallInDBox()==true)
	  {
		  sParam.GoToPointP.x =  -HALF_FIELD_MAXX*0.6;
		  sParam.GoToPointP.y =  state->ballPos.y  - SGN(state->ballPos.y)*2*BOT_RADIUS;
	   
	  
	  }
	else
	{*/
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
		//sParam.GoToPointP.x = state->homePos[index].x;//- BOT_RADIUS;
	  //sParam.GoToPointP.y = state->homePos[index].y-SGN(state->homePos[index].y)*2*BOT_RADIUS;
	  //sParam.GoToPointP.finalslope =  0.9*(state->homePos[index].angle) ; 
	  sParam.GoToPointP.align = true;
	  sParam.GoToPointP.increaseSpeed = true;
	//}
	 
	  //sParam.GoToPointP.finalVelocity = 100*MAX_BOT_SPEED;
	   skillSet->executeSkill(sID, sParam);


    }//; // class TAttack*/
  
	bool isBallInDBox()
  {
      if((abs(state->ballPos.y)<0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < -HALF_FIELD_MAXX*0.7)) 
          return true;

		  return false;

  
  }
    }; // namespace MyStrategy
}
#endif // TTCharge_HPP