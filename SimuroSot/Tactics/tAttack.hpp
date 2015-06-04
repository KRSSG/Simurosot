#ifndef TTATTACK_HPP
#define TTATTACK_HPP
 
#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include "../common/include/geometry.hpp"
#include "../Utils/intersection.hpp"
#include "../winDebugger/Client.h"

#define ANGLE_TO_DIST 0
#define MIN(a,b) (a) < (b) ? (a) : (b)
namespace MyStrategy
{
  class TAttack : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TAttack(const BeliefState* state, int botID) :
      Tactic(Tactic::Attack, state, botID)
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

    ~TAttack()
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
//***********
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
Vector2D<int> decidedGoalPoint(){
int id=opponentProbableGoalkeeper();
Vector2D<int> pointToAttack;
if(id==-1){
	  pointToAttack.x=HALF_FIELD_MAXX-GOAL_DEPTH;
	  pointToAttack.y=0;
	  return pointToAttack;
}
Vector2D<int> pointOfAttack(HALF_FIELD_MAXX-GOAL_DEPTH, 0);
float errorAngle=0.08726*2;
float angleGoalMax_y, angleGoalMin_y;
Vector2D<int> goal_maxy(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MAXY);
Vector2D<int> goal_miny(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MINY);
angleGoalMax_y=Vector2D<int>::angle(state->ballPos , goal_maxy)-errorAngle;
angleGoalMin_y=Vector2D<int>::angle(state->ballPos, goal_miny)+errorAngle;
if(state->awayPos[id].x > 0.6*OPP_GOAL_MAXY){
pointOfAttack.y=angleGoalMin_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
return pointOfAttack;
}
if(state->awayPos[id].x < 0.6*OPP_GOAL_MINY){
   pointOfAttack.y=angleGoalMax_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
return pointOfAttack;
}
if(state->awayPos[id].y >=0 && state->awayVel[id].y >=0 ){
//	return angleGoalMin_y;
pointOfAttack.y=angleGoalMin_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
return pointOfAttack;
}
else if(state->awayPos[id].y <0 && state->awayVel[id].y <0){
//return angleGoalMax_y;
pointOfAttack.y=angleGoalMax_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
   return pointOfAttack;
}
return pointOfAttack; 
}
   void shoot(){
    Vector2D<int> targetGoalPoint=decidedGoalPoint();
	Vector2D<int> ballPredictedPos;
	Vector2D<float> ballTransformedVel;
	float factor = 0.00005;
	float botBallAngle=Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
	float botBallDist=Vector2D<int>::dist(state->homePos[botID], state->ballPos);
	float ballGoalPointAngle=Vector2D<int>::angle(ballPredictedPos, targetGoalPoint);
	//* why is ballPredictedPos being used before its assignment ??!!!!!

	int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);

    ballPredictedPos.x = state->ballPos.x ;//+ (int)ballBotDist * factor * state->ballVel.x;
    ballPredictedPos.y = state->ballPos.y ;//+ (int)ballBotDist * factor * state->ballVel.y;
 //for spinnig inside other's    D_BOX
/* if(ballPredictedPos.x < -HALF_FIELD_MAXX/2){
	 ballPredictedPos.x=-HALF_FIELD_MAXX/2;
	 ballPredictedPos.y+=SGN(state->ballPos.y)*BOT_RADIUS;
 }*/
 sID=SkillSet::GoToPoint;
 sParam.GoToPointP.x=ballPredictedPos.x;
 sParam.GoToPointP.y=ballPredictedPos.y;
/* if(botBallDist < BOT_RADIUS){
   sParam.GoToPointP.x=targetGoalPoint.x;
   sParam.GoToPointP.y=targetGoalPoint.y;
}*/
 sParam.GoToPointP.align=true;
 sParam.GoToPointP.finalslope=ballGoalPointAngle;
 sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;
 skillSet->executeSkill(sID,sParam);
 return;
}
   bool isBallInDBox()
  {
      if((abs(state->ballPos.y)<0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < -HALF_FIELD_MAXX*0.7)) 
          return true;

		  return false;

  
  }
    void execute(const Param& tParam)
    {	
		//shoot(); 
		//return;
		float a1=Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
		float a2=Vector2D<int>::angle(Vector2D<int> (HALF_FIELD_MAXX-GOAL_DEPTH,0),state->ballPos);
	/*	if((a1-a2) >-PI/10 && (a1-a2)<PI/10 && state->homePos[botID].x <state->ballPos.x && SGN(state->ballPos.y)*SGN(state->ballVel.y)==-1){
			shoot();
			return;
		}*/
	  char debug[250];
	  sParam.GoToPointP.trapezoidal = false;
	  sprintf(debug,"In tAttack\n");
	  ////Client::debugClient->SendMessages(debug);
      float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
      //movementError[movementErrorIndex++] = (Vector2D<int>::distSq(prevBotPos, state->homePos[botID])) + (prevBotAngle - state->homeAngle[botID])*(prevBotAngle - state->homeAngle[botID])*50000;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      /*movementErrorIndex %= 10;
      movementErrorSum = 0;
      for(int i=0; i<10; i++)
        movementErrorSum += movementError[i];
      if(movementErrorSum < 300 && tParam.AttackP.rotateOnError)
      {
        sID = SkillSet::Spin;
        sParam.SpinP.radPerSec = MAX_BOT_OMEGA * (state->homePos[botID].y > 0? ForwardX(1): ForwardX(-1));
        skillSet->executeSkill(sID, sParam);
        return;
      }*/
      sprintf(debug,"%d %d %d %d %d %d\n",APPROACHING,
      SPINNING_CCW ,
      SPINNING_CW,
      ATTACKING,
      CLOSE_TO_BALL,
      STUCK);
		////Client::debugClient->SendMessages(debug);
		//iState = SPINNING_CCW;
		sprintf(debug,"Current State %d\n",iState);
		////Client::debugClient->SendMessages(debug);
          
      switch(iState)
      {
        
        case APPROACHING:
         {
          /* Ball is not with bot so go to ball first */
                    sprintf(debug,"dist = %f\n",dist);
                        ////Client::debugClient->SendMessages(debug);
          if(dist<BOT_BALL_THRESH)
          {
            iState = CLOSE_TO_BALL ;
            break;
          }
		 // shoot();
		 //  break;
                   sprintf(debug,"i am here 1\n");
                        ////Client::debugClient->SendMessages(debug);
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
                  sprintf(debug,"ballgoaldist = %f\n",ballgoaldist);
                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
         
		  //******************** changed**********************
		  float factorx = 0.00005;
		  if(state->ballVel.x<200  )
			  factorx=0.00005;
		  else if(state->ballVel.x<1000  )
			  factorx=0.00005;
		  else if(state->ballVel.x<1400)
			  factorx=0.00006;
		  else factorx=0.00008;

		   float factory = 0.00005;
		  if( state->ballVel.y<200 )
			  factory=0.00002;
		  else if(state->ballVel.y<1000 )
			  factory=0.00004;
		  else if(state->ballVel.y<1400)
			  factory=0.00006;
		  else factory=0.00008;
		  //****************************************************
		  //factorx=factory=0.00005;

          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factorx * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factory * state->ballVel.y;
          int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.
                  sprintf(debug,"i am here 2\n");
                        //Client::debugClient->SendMessages(debug);
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
                          sprintf(debug,"i am here 3\n");
                      // Client::debugClient->SendMessages(debug);
            if(!LocalAvoidance::isPointInField(state->ballPos))
            {
              offset= 0;
              x3 =  (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
              y3 =  (targetY * (ballgoaldist + offset)) / ballgoaldist;
              break;
            }
            offset /= 1.25;
            if(offset <= 1.0)
              break;
            x3 =  (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
              y3 =  (targetY * (ballgoaldist + offset)) / ballgoaldist;
          }
          offset/=1.25;
         
          sprintf(debug,"i am here 5\n");
                  ////Client::debugClient->SendMessages(debug);
          sprintf(debug,"x3 = %d y3 = %d ballVel x = %f y = %f\n",x3,y3,state->ballVel.x,state->ballVel.x,state->ballVel.y);
          //Client::debugClient->SendMessages(debug);
          Vector2D<int> offsetpt(x3, y3);
          int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
          if(dist2 < 300)
            hasAchievedOffset = 1;
                  /* Bot is already between ball and offset point */
                  //else if(Vector2D<int>::dist(offsetpt,state->ballPos) < 500 + Vector2D<int>::dist(state->homePos[botID],state->ballPos) + dist2) hasAchievedOffset = 1; 
          else if(dist > 2 * offset)
            hasAchievedOffset = 0; 

          if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x)) 
            hasAchievedOffset = 0;
            
          sParam.GoToPointP.x = x3;
          sParam.GoToPointP.y = y3;
                  sprintf(debug,"x3 = %d y3 = %d\n",x3,y3);
                  ////Client::debugClient->SendMessages(debug);
          sprintf(debug,"i am here 6\n");
                  ////Client::debugClient->SendMessages(debug);
          sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
          sParam.GoToPointP.increaseSpeed = 0;
          if(hasAchievedOffset)
          {
                          //Client::debugClient->SendMessages("yee i have achived offset!!!!!!!!!!!!\n");
            sParam.GoToPointP.x = state->ballPos.x;
            sParam.GoToPointP.y = state->ballPos.y;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle( state->ballPos,state->homePos[botID]);
            sParam.GoToPointP.increaseSpeed = 1;

          }
		    else sParam.GoToPointP.trapezoidal = true;

			sprintf(debug,"Going to point %f %f\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
			//Client::debugClient->SendMessages(debug);
			sParam.GoToPointP.align = false;
      if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) 
		  sParam.GoToPointP.align = true;
   //resricting     
	/*  if((sParam.GoToPointP.x < -HALF_FIELD_MAXX/2)&&(abs(state->ballPos.y)<0.6*HALF_FIELD_MAXY))

		{ 
			sParam.GoToPointP.x  = ForwardX(-HALF_FIELD_MAXX/2);
			
			if(abs(state->ballPos.y) > HALF_FIELD_MAXY*0.6)
			  sParam.GoToPointP.y = state->ballPos.y - SGN(state->ballPos.y)*2*BOT_RADIUS;
			else
              sParam.GoToPointP.y = state->ballPos.y + SGN(state->ballPos.y)*2*BOT_RADIUS;

	    }
    */

	   if(isBallInDBox()==true)
	  {
		  sParam.GoToPointP.x =  -HALF_FIELD_MAXX*0.5;
		  sParam.GoToPointP.y =  state->ballPos.y  - SGN(state->ballPos.y)*2*BOT_RADIUS;
	  }


	  skillSet->executeSkill(sID, sParam);
          
        break;
         }

        case SPINNING_CW:
          {  
          if(dist>BOT_BALL_THRESH)
          {
            iState = APPROACHING;
            return;
          }
		 // shoot();
		 // break;
          sID = SkillSet::Spin;
          if(FIELD_IS_INVERTED == false)
            sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
		  else
			  sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
          skillSet->executeSkill(sID, sParam);
        break;    

          }
        case SPINNING_CCW:
        {
          if(dist>BOT_BALL_THRESH)
          {
            iState = APPROACHING;
            return;
          }
         // shoot();
		 // break;
          sID = SkillSet::Spin;
		        if(FIELD_IS_INVERTED == false)
                  sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
                else
				  sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
             
          skillSet->executeSkill(sID, sParam);
        break;
        }

        case CLOSE_TO_BALL:
        {
          if(fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 20 && fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 20)
          {
            if(state->ballPos.y > 0)
              iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;        //opposite of false
            else
              iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;        //opposite of flase
            break;
          }
          /* Ball is with bot. So go to goal */
            sID = SkillSet::GoToPoint;
			int desty = 0;
			//Client::debugClient->SendMessages("ATTACKING\n");
			/*if(state->ballVel.y<200 && state->ballVel.x<0 && abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_RADIUS )
			{
				int i=50;
				while(--i);
			}*/
			Vector2D<int> GoalMidPoint  (OPP_GOAL_X,0);
			Vector2D<int> GoalLeftPoint (OPP_GOAL_X,OPP_GOAL_MINY);
			Vector2D<int> GoalRightPoint(OPP_GOAL_X,OPP_GOAL_MAXY);
			float angleofBot = state->homeAngle[botID];
			if(angleofBot > (3 * PI)/4) angleofBot  = (3 * PI)/4;
			if(angleofBot < -(3 * PI)/4) angleofBot = -(3 * PI)/4;
			float R =  (OPP_GOAL_X - state->homePos[botID].x) / ( cos(angleofBot) );
			float destY = state->homePos[botID].y + R * sin(angleofBot);
			sprintf(debug,"Angle with Goal : %f  Dest Y :  %f\n",angleofBot,destY);
			if(destY < OPP_GOAL_MINY + 200) destY = OPP_GOAL_MINY + 200;
			if(destY > OPP_GOAL_MAXY - 200) destY = OPP_GOAL_MAXY - 200;
			/*float angleWithGoal  =  Vector2D<int>::angle(state->homePos[botID],GoalMidPoint);
			float leftGoalAngle  =  Vector2D<int>::angle(GoalLeftPoint,state->homePos[botID]);
			float rightGoalAngle =  Vector2D<int>::angle(state->homePos[botID],GoalRightPoint);
            */sParam.GoToPointP.align = false;
            sParam.GoToPointP.x = OPP_GOAL_X;
            sParam.GoToPointP.y = destY;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
            sParam.GoToPointP.increaseSpeed = 1;
            skillSet->executeSkill(sID, sParam);
            tState = RUNNING;
            iState = CLOSE_TO_BALL; //ATTACKING;
            if(dist > BOT_BALL_THRESH)
            {
              iState = APPROACHING ;
            }
          

          break;
          
        }
      }       

 
    }; // class TAttack
    }; // namespace MyStrategy
}
#endif // TTCharge_HPP