#ifndef TTATTACK_HPP
#define TTATTACK_HPP
 
#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
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

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
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
   // printf("%d >>>>>>>>>> %f , %f\n", *it,dis_from_ball,angle_difference);
        weight = dis_from_ball + ANGLE_TO_DIST * angle_difference;
        //if(*it == botID)
      //    weight -= HYSTERESIS;
        if(weight < minwt)
        {
          minwt = dis_from_ball ;
          minv = *it;
        }
      }
	 if((minv==3)||(minv==4))
		 return minv;
	 else
	 {
	   int dis3 , dis4;
	   dis3 = Vector2D<int>::dist(state->homePos[3],state->ballPos);
	   dis4 = Vector2D<int>::dist(state->homePos[4],state->ballPos);
	   if(dis4<dis3)
		   return 4;
	   else 
		   return 3;

	 
	 }
     // Util::// LoggertoStdOut("Selected bot %d\n", minv);
     

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
void revolve(float &v_l , float &v_r ,const BeliefState* state, int botID) {
	
	float ballBotDist = Vector2D<int>::dist(state->homePos[botID],state->ballPos);
	float ballSpeed = sqrt((int)(state->ballVel.x)^2 + (int)(state->ballVel.y)^2);
    float botSpeed  = sqrt((int)(state->homeVel[botID].x)^2 + (int)(state->homeVel[botID].y)^2);
	int timefactor = (2*ballBotDist + 2*BOT_RADIUS)*(PI/2)/((2*0.6*MAX_BOT_SPEED)-(ballSpeed*(PI/2))) ;
	float radiusRotation = ballBotDist + 0.5*ballSpeed*timefactor ;
	if(state->ballPos.x < state->homePos[botID].x)
	{
	  if(state->ballPos.y > 0)
	  {
		v_r = 2*(0.6*MAX_BOT_SPEED)*(radiusRotation)/((2*radiusRotation)+ (2*BOT_RADIUS)) ;
		v_l = 2*(0.6*MAX_BOT_SPEED)*((radiusRotation) + (2*BOT_RADIUS))/((2*radiusRotation)+ (2*BOT_RADIUS)) ;
	  }
	  else
       {
		v_l = 2*(0.6*MAX_BOT_SPEED)*(radiusRotation)/((2*radiusRotation)+ (2*BOT_RADIUS)) ;
		v_r = 2*(0.6*MAX_BOT_SPEED)*((radiusRotation) + (2*BOT_RADIUS))/((2*radiusRotation)+ (2*BOT_RADIUS)) ;
	  }
	}
	printf(" ballBotdist ");
}  
void shoot(const BeliefState* state, int botID){

     cout << "my case" << endl;	
 Vector2D<int> targetGoalPoint=decidedGoalPoint_new();
 Vector2D<int> ballPredictedPos;
 Vector2D<float> ballTransformedVel;
 float factor = 0.00005;
 float botBallAngle=Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
 float botBallDist=Vector2D<int>::dist(state->homePos[botID], state->ballPos);
 float ballGoalPointAngle=Vector2D<int>::angle(ballPredictedPos, targetGoalPoint);
 float cosTheta=(1/(sqrt(1+ballGoalPointAngle*ballGoalPointAngle)));
 float sinTheta=SGN(ballGoalPointAngle)*(ballGoalPointAngle/(sqrt(1+ballGoalPointAngle*ballGoalPointAngle)));
 int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
 float ballAngle ;
 if(state->ballVel.x < 10) 
	 if(state->ballVel.y > 0)
		 ballAngle = PI/2 ;
	 else
		 ballAngle = -PI/2 ;
 else
	 ballAngle = state->ballVel.y/state->ballVel.x ;
 
 ballPredictedPos.x = state->ballPos.x - (int)(((botBallDist*factor*state->ballVel.x))/(sqrt(state->ballVel.x*state->ballVel.x+state->ballVel.y*state->ballVel.y)));//(int)HALF_FIELD_MAXX*0.05;
 float ballSpeed = sqrt((int)(state->ballVel.x)^2 + (int)(state->ballVel.y)^2);
 float botSpeed  = sqrt((int)(state->homeVel[botID].x)^2 + (int)(state->homeVel[botID].y)^2);
 float numerator = ((ballSpeed*sin(ballAngle-state->homeAngle[botID])*cos(state->homeAngle[botID]))+(botSpeed*sin(state->homeAngle[botID])));
 float denominator = ((-1*ballSpeed*sin(ballAngle-state->homeAngle[botID])*sin(state->homeAngle[botID]))+(botSpeed*cos(state->homeAngle[botID]))) ;
 ballPredictedPos.y = state->homePos[botID].y + ((numerator/denominator)*(ballPredictedPos.x - state->homePos[botID].x))	; 
	 //******************************* new shoot algo implemented ********************************************* 
  //    ballPredictedPos.x = state->ballPos.x - (int)(((botBallDist*factor*state->ballVel.x))/(sqrt(state->ballVel.x*state->ballVel.x+state->ballVel.y*state->ballVel.y)));//(int)HALF_FIELD_MAXX*0.05;
 //     ballPredictedPos.y = state->ballPos.y + ((int)((ballBotDist*factor*state->ballVel.y))/(sqrt(state->ballVel.x*state->ballVel.x+state->ballVel.y*state->ballVel.y))) ;//(int)HALF_FIELD_MAXX*0.05;

if(ballPredictedPos.x < -HALF_FIELD_MAXX+GOAL_DEPTH)
 ballPredictedPos.x=-HALF_FIELD_MAXX + GOAL_DEPTH;

 if(ballPredictedPos.x >HALF_FIELD_MAXX-GOAL_DEPTH)
 ballPredictedPos.x=HALF_FIELD_MAXX-GOAL_DEPTH;

 if(ballPredictedPos.y >HALF_FIELD_MAXY)
 ballPredictedPos.y=HALF_FIELD_MAXY;

 if(ballPredictedPos.y < -HALF_FIELD_MAXY)
 ballPredictedPos.y=-HALF_FIELD_MAXY;

 //for spinnig inside other's    D_BOX
 if((state->ballPos.x >(HALF_FIELD_MAXX - GOAL_DEPTH - 0.7*BOT_RADIUS - 2*BOT_RADIUS))&&(abs(state->ballPos.y)<OUR_GOAL_MAXY-20)&&(botBallDist < 1.2*BOT_BALL_THRESH))
 {
         sID = SkillSet::Spin;
 if(state->ballPos.y > state->homePos[botID].y)
      sParam.SpinP.radPerSec = MAX_BOT_OMEGA;
 else
  sParam.SpinP.radPerSec = -MAX_BOT_OMEGA;
             skillSet->executeSkill(sID, sParam);
 }
 
 sID=SkillSet::GoToPoint;
 sParam.GoToPointP.x=ballPredictedPos.x;
 sParam.GoToPointP.y=ballPredictedPos.y;
 if(iState==CLOSE_TO_BALL){
   sParam.GoToPointP.x=targetGoalPoint.x;
   sParam.GoToPointP.y=targetGoalPoint.y;
}
 sParam.GoToPointP.align=false;
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
		static Vector2D<int> prevPos(0,0) ;
		static int counter = 0;
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
		float prevDist = Vector2D<int>::dist(prevPos,state->homePos[botID])<2*BOT_BALL_THRESH ;
		
		// if stuck then do this 
	/*
	if((prevDist<BOT_BALL_THRESH)&&((abs(state->homeVel[botID].x)>10)||(abs(state->homeVel[botID].y)>10)))
			counter++;
		if(((prevDist<BOT_BALL_THRESH)&&((abs(state->homeVel[botID].x)>10)||(abs(state->homeVel[botID].y)>10)))&&(counter>50))
		{
			sID = SkillSet::Velocity;
			sParam.VelocityP.vl = -1*SGN(state->homeVel[botID].y)*100 ;
			sParam.VelocityP.vr = -1*SGN(state->homeVel[botID].y)*100 ;
			 sprintf(debug,"stuck \n");
             Client::debugClient->SendMessages(debug);
			 counter = 0 ; 
			skillSet->executeSkill(sID, sParam);
		    
		}
	   else
	 */
      switch(iState)
      {  
         counter = 0 ;
         prevPos.x = state->homePos[botID].x ; prevPos.y = state->homePos[botID].y ;
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
		  float ballgoaldist = Vector2D<int>::dist(state->ballPos, decidedGoalPoint_new());// Vector2D<int>(OPP_GOAL_X, 0));
                  sprintf(debug,"ballgoaldist = %f\n",ballgoaldist);
                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
          float factor = 0.00005;
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
		  int x3 = (targetX * (ballgoaldist + offset) - offset * decidedGoalPoint_new().x/*OPP_GOAL_X*/) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.
                  sprintf(debug,"i am here 2\n");
                        //Client::debugClient->SendMessages(debug);
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
                          sprintf(debug,"i am here 3\n");
                        //Client::debugClient->SendMessages(debug);
            if(!LocalAvoidance::isPointInField(state->ballPos))
            {
              offset= 0;
              x3 =  (targetX * (ballgoaldist + offset) - offset * decidedGoalPoint_new().x/*OPP_GOAL_X*/) / ballgoaldist;
              y3 =  (targetY * (ballgoaldist + offset)) / ballgoaldist;
              break;
            }
            offset /= 1.25;
            if(offset <= 1.0)
              break;
              x3 =  (targetX * (ballgoaldist + offset) - offset * decidedGoalPoint_new().x/*OPP_GOAL_X*/) / ballgoaldist;
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
          
		  if((abs(state->ballPos.y) > 0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < state->homePos[botID].x)&&(ballBotDist<4*BOT_BALL_THRESH))
		  {  
			  /*
			  float v_l , v_r ;
			  sID = SkillSet::Velocity;  
			  revolve(v_l,v_r,state,botID) ;
			  sParam.VelocityP.vl = v_l ;
			  sParam.VelocityP.vr = v_r ;
			  sprintf(debug,"v_l = %d , v_r = %d",v_l,v_r);
			  Client::debugClient->SendMessages(debug);
			  skillSet->executeSkill(sID, sParam);
			  break ;
			  */
			  
			  sID = SkillSet::GoToPoint ;
			  sParam.GoToPointP.x = state->ballPos.x - abs(state->ballVel.x)*((int)ballBotDist*40*factor);
			  sParam.GoToPointP.y = state->ballPos.y - SGN(state->ballPos.y)*3*BOT_BALL_THRESH ;
              sprintf(debug,"going into condition");
			  Client::debugClient->SendMessages(debug);
			  skillSet->executeSkill(sID,sParam) ;
		      break ;
		  
		  }
		    /*
		    else
			 if(state->ballPos.x > state->homePos[botID].x)
			   {
			    shoot(state,botID) ;
			    break ;
			   }
			 */
			 sParam.GoToPointP.x = x3;
             sParam.GoToPointP.y = y3;
                  sprintf(debug,"x3 = %d y3 = %d\n",x3,y3);
                  ////Client::debugClient->SendMessages(debug);
          sprintf(debug,"i am here 6\n");
                  ////Client::debugClient->SendMessages(debug);
          sParam.GoToPointP.finalslope = Vector2D<int>::angle( decidedGoalPoint_new()/*Vector2D<int>(OPP_GOAL_X, 0)*/,state->ballPos);
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
		  sParam.GoToPointP.y =  state->ballPos.y  - SGN(state->ballPos.y)*3*BOT_RADIUS;
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
          //shoot();  
		  /*
		  if((a1-a2) >-PI/10 && (a1-a2)<PI/10 && state->homePos[botID].x <state->ballPos.x && SGN(state->ballPos.y)*SGN(state->ballVel.y)==-1){
			shoot(state,botID);
			return;
		  }
		  */
			sID = SkillSet::GoToPoint;
		/*	int desty = 0;
			//Client::debugClient->SendMessages("ATTACKING\n");
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
			*/
			
			/*float angleWithGoal  =  Vector2D<int>::angle(state->homePos[botID],GoalMidPoint);
			float minGoalAngle  =  Vector2D<int>::angle(GoalLeftPoint,state->homePos[botID]);
			float maxGoalAngle =  Vector2D<int>::angle(state->homePos[botID],GoalRightPoint);
            */
			
			
			sParam.GoToPointP.align = false;
			sParam.GoToPointP.x = decidedGoalPoint_new().x;
			sParam.GoToPointP.y = decidedGoalPoint_new().y;
			if(sParam.GoToPointP.y < OPP_GOAL_MINY + 200) sParam.GoToPointP.y = OPP_GOAL_MINY;
			if(sParam.GoToPointP.y > OPP_GOAL_MAXY - 200) sParam.GoToPointP.y = OPP_GOAL_MAXY;
            sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(decidedGoalPoint_new().x,decidedGoalPoint_new().y),state->ballPos);
            sParam.GoToPointP.increaseSpeed = 1;
            skillSet->executeSkill(sID, sParam);
			
            tState = RUNNING;
            iState = CLOSE_TO_BALL; //ATTACKING;
            if(dist > BOT_BALL_THRESH){
              iState = APPROACHING ;
            }
          

          break;
          
        }
      }       

 
    }; // class TAttack
    }; // namespace MyStrategy
}
#endif // TTCharge_HPP