

#ifndef TTATTACKNEW_HPP
#define TTATTACKNEW_HPP
 
#define GUNJAN 

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
  class TAttackNew : public Tactic
  {
    static const int offset = 400;
    // Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TAttackNew(const BeliefState* state, int botID) :
      Tactic(Tactic::AttackNew, state, botID)
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

    ~TAttackNew()
    { } // ~TAttack
    enum InternalState
    {
      APPROACHING ,
      SPINNING_CCW ,
      SPINNING_CW,
      SHOOTING,
      STEAL,
      PASSING,
    } iState;
    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

 inline float todegree(float ang) {
	 return (180/PI)*ang ;
 }
 inline float toradian(float ang) {
      return (PI/180)*ang ;
 }

 inline float angletransform(float ang){
  if(ang > PI/2)
	  return (PI - ang);
  else if(ang < -PI/2)
	  return (PI + ang);
  else 
	  return ang ;
 }

  bool pointxInField(Vector2D<int> final)
    {
      // checks if the point is in the field 
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

  int opponentProbableGoalkeeper() {
// returns the id of opponent goal keeper 
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
    char debug[250] ;
	  int id=opponentProbableGoalkeeper();
    Vector2D<int> pointToAttack;
    if(id==-1){
	  pointToAttack.x=HALF_FIELD_MAXX-GOAL_DEPTH;
	  pointToAttack.y=0;
	  return pointToAttack;
    }
  Vector2D<int> pointOfAttack(HALF_FIELD_MAXX-GOAL_DEPTH, 0);
  float errorAngle= toradian(5);     //given angle in degrees ;
  float angleGoalMax_y, angleGoalMin_y;
  Vector2D<int> goal_maxy(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MAXY);
  Vector2D<int> goal_miny(HALF_FIELD_MAXX-GOAL_DEPTH, OPP_GOAL_MINY);
  angleGoalMax_y=Vector2D<int>::angle(state->ballPos , goal_maxy) - errorAngle;
  angleGoalMin_y=Vector2D<int>::angle(state->ballPos , goal_miny) + errorAngle;
  if(state->awayPos[id].y > 0.6*OPP_GOAL_MAXY){
      pointOfAttack.y= tan(angleGoalMin_y)*(pointOfAttack.x-ForwardX(state->ballPos.x)) + state->ballPos.y;
	  if(abs(pointOfAttack.y) > OUR_GOAL_MAXY)
		  pointOfAttack.y = SGN(pointOfAttack.y)*(OUR_GOAL_MAXY - BOT_RADIUS) ;
      return pointOfAttack;
  }
  if(state->awayPos[id].y < 0.6*OPP_GOAL_MINY){
      pointOfAttack.y=tan(angleGoalMax_y)*(pointOfAttack.x-ForwardX(state->ballPos.x))  + state->ballPos.y;
	  if(abs(pointOfAttack.y) > OUR_GOAL_MAXY)
		  pointOfAttack.y = SGN(pointOfAttack.y)*(OUR_GOAL_MAXY - BOT_RADIUS) ;
	  return pointOfAttack;
  }
  if(state->awayPos[id].y >=0 || state->awayVel[id].y >=0 ){
      pointOfAttack.y=tan(angleGoalMin_y)*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
	  if(abs(pointOfAttack.y) > OUR_GOAL_MAXY)
		  pointOfAttack.y = SGN(pointOfAttack.y)*(OUR_GOAL_MAXY - BOT_RADIUS) ;
	  return pointOfAttack;
  }
  if(state->awayPos[id].y < 0 || state->awayVel[id].y < 0 ){
   pointOfAttack.y=angleGoalMax_y*(pointOfAttack.x-ForwardX(state->ballPos.x))-state->ballPos.y;
	  if(abs(pointOfAttack.y) > OUR_GOAL_MAXY)
		  pointOfAttack.y = SGN(pointOfAttack.y)*(OUR_GOAL_MAXY - BOT_RADIUS) ;   
   return pointOfAttack;
  }
	  if(abs(pointOfAttack.y) > OUR_GOAL_MAXY)
		  pointOfAttack.y = SGN(pointOfAttack.y)*(OUR_GOAL_MAXY - BOT_RADIUS) ;
  return pointOfAttack; 
}
  
  Vector2D<int> predictBall() {
        static int i=0;
			static Vector2D<int> pos[3] = {Vector2D<int>(state->ballPos.x , state->ballPos.y),Vector2D<int>(state->ballPos.x , state->ballPos.y),Vector2D<int>(state->ballPos.x , state->ballPos.y)};
			static Vector2D<int> dest(state->ballPos.x , state->ballPos.y ) ;
			float dx1,dx2,dy1,dy2,dx,dy;
		float factorx ;
		  if(state->ballVel.x<200  )
			  factorx=0.00001;
		  if(state->ballVel.x<500  )
			  factorx=0.000015;
		  else if(state->ballVel.x<1000  )
			  factorx=0.00002;
		  else if(state->ballVel.x<1500)
			  factorx=0.000025;
		  else if(state->ballVel.x<2000)
			  factorx=0.000035;
		  else if(state->ballVel.x<3000)
			  factorx=0.00004;
		  else factorx=0.0003;

		   float factory;
		  if( state->ballVel.y<200 )
			  factory=0.00001;
		  if( state->ballVel.y<500 )
			  factory=0.000015;
		  else if(state->ballVel.y<1000 )
			  factory=0.00002;
		  else if(state->ballVel.y<1500)
			  factory=0.000025;
		  else if(state->ballVel.y<2000)
			  factory=0.000035;
		  else if(state->ballVel.y<3000)
			  factory=0.00004;
		  else factory=0.000;
			

			if(i>2)
			{
				i--;
				pos[0]=pos[1];
				pos[1]=pos[2];
			}

			pos[i].x=state->ballPos.x;
			pos[i].y=state->ballPos.y;

			i++;

			dx1=pos[1].x-pos[0].x;
			dx2=pos[2].x-pos[1].x;
			dx=(dx1+dx2)/2;

			dy1=pos[1].y-pos[0].y;
			dy2=pos[2].y-pos[1].y;
			dy=(dy1+dy2)/2;

			dest.x=state->ballPos.x+dx*factorx*state->ballVel.x;
			dest.y=state->ballPos.y+dy*factory*state->ballVel.y;
	  /* 
	    static int i=0;
		static Vector2D<int> pos[3] = {Vector2D<int>(state->ballPos.x , state->ballPos.y),Vector2D<int>(state->ballPos.x , state->ballPos.y),Vector2D<int>(state->ballPos.x , state->ballPos.y)};
		static Vector2D<int> dest(state->ballPos.x , state->ballPos.y ) ;
		float dx1,dx2,dy1,dy2,dx,dy;
		
		pos[i].x=state->ballPos.x;
		pos[i].y=state->ballPos.y;
		
		i = (i+1)%3 ;

			    dx1=pos[1].x-pos[0].x;
				dx2=pos[2].x-pos[1].x;
				dx=(dx1+dx2)/2;

				dy1=pos[1].y-pos[0].y;
				dy2=pos[2].y-pos[1].y;
				dy=(dy1+dy2)/2;
			
				dest.x=state->ballPos.x+dx;
				dest.y=state->ballPos.y+dy;
	   */
		
		   return dest ;
  
  }
  
  bool isBallInDBox()
  {
      if((abs(state->ballPos.y)<0.8*HALF_FIELD_MAXY)&&(state->ballPos.x < -HALF_FIELD_MAXX*0.7)) 
          return true;

		  return false;

  
  }

  void shoot(){
    //Vector2D<int> targetGoalPoint=decidedGoalPoint();
	Vector2D<int> ballPredictedPos;
	Vector2D<float> ballTransformedVel;
	float factor = 0.00005;
	float botBallAngle=Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
	float botBallDist=Vector2D<int>::dist(state->homePos[botID], state->ballPos);
	//* why is ballPredictedPos being used before its assignment ??!!!!!

	int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);

   // ballPredictedPos.x = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
   // ballPredictedPos.y = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
	//float ballGoalPointAngle=Vector2D<int>::angle(ballPredictedPos, targetGoalPoint);

   sID=SkillSet::GoToPointDW;
   sParam.GoToPointDWP.x=state->ballPos.x ; //targetGoalPoint.x;
   sParam.GoToPointDWP.y=state->ballPos.y ; //targetGoalPoint.y;
/* if(botBallDist < BOT_RADIUS){
   sParam.GoToPointP.x=targetGoalPoint.x;
   sParam.GoToPointP.y=targetGoalPoint.y;
}*/
 //sParam.GoToPointDWP.finalslope=ballGoalPointAngle;
 skillSet->executeSkill(sID,sParam);
 return;
}



  void execute(const Param& tParam)
  {	  char debug[250];
	  float ballbotdist  = Vector2D<int>::dist( state->ballPos , state->homePos[botID]);
	  float ballbotAngle = Vector2D<int>::angle(state->ballPos , state->homePos[botID]);
	  float oppballdist  = Vector2D<int>::dist( state->ballPos , state->awayPos[state->oppBotNearestToBall]) ;
	  Vector2D<int> ballPredictPos = predictBall() ;
	  oppballdist = BOT_BALL_THRESH ;
	  Vector2D<int> goalPoint = decidedGoalPoint() ; 
	  int safeDist = 2*BOT_BALL_THRESH ;
	  Vector2D<int> approachPoint ;
	  float destAngle = Vector2D<int>::angle(goalPoint,state->ballPos) ;
	  approachPoint.x = predictBall().x - (safeDist*cos(destAngle));
	  approachPoint.y = predictBall().y - (safeDist*sin(destAngle));

	  // use loca avoidance for the approachpoint . imp
	  
	  int pointbotdist = Vector2D<int>::dist( approachPoint , state->homePos[botID] );
	  switch(iState)
	  {
	   case APPROACHING :
	   {   
		  

		   //change condition for shooting
		   //search for different shooting techniques
		   static bool firstapproach = true ;
		   int thresh = 1.5*BOT_BALL_THRESH ;
		   if(firstapproach)
		   {
			   if(pointbotdist > 5*BOT_BALL_THRESH)
				   thresh = 2*safeDist ;
			   else
				   thresh = 0.5*safeDist ;
			   firstapproach = FALSE ;
		   }
		   if((pointbotdist<thresh)&&(state->ballPos.x > state->homePos[botID].x)/*&&(abs(angletransform(abs(state->homeAngle[botID]-destAngle)))<toradian(10))*/)
           {
            iState = SHOOTING ;
			firstapproach = TRUE ;
			break;
           }
		   if((oppballdist < 2.5*BOT_RADIUS)&&(state->ballPos.x < state->homePos[botID].x - BOT_BALL_THRESH ))
		   {
		    // Go and steal the ball 
			iState = STEAL ;
			firstapproach = TRUE ;
			break ;
		   }
		   /*
		   else
		   {
			   if((abs(state->homeAngle[botID]-destAngle)>toradian(10))&&(ballbotdist<1.5*safeDist)&&(state->ballPos.x > state->homePos[botID].x))
			   {
				   sID = SkillSet::TurnToAngle ;
				   sParam.TurnToAngleP.finalslope = destAngle ;
				   skillSet->executeSkill(sID,sParam) ;
				   break ;
			   }
		   }
		   */
		   if(state->ballPos.x > state->homePos[botID].x + 0.5*BOT_BALL_THRESH)
		   {

		    sprintf(debug,"BallPos :: ( %d , %d ) \n ApproachPoint :: (%d , %d ) \n\n",state->ballPos.x,state->ballPos.y,approachPoint.x,approachPoint.y);
		    Client::debugClient->SendMessages(debug);
		    sID = SkillSet::GoToPoint ;
		    sParam.GoToPointP.x = approachPoint.x ;
		    sParam.GoToPointP.y = approachPoint.y ;
		    sParam.GoToPointP.finalslope = destAngle ;
		    sParam.GoToPointP.finalVelocity = 0 ;
		    skillSet->executeSkill(sID, sParam);
	        break;
	       }
		   else
		   {
		    /*approachPoint.x = state->ballPos.x - (safeDist*cos(destAngle));
		    approachPoint.y = state->ballPos.y - (safeDist*sin(destAngle));
		    sID = SkillSet::GoToPointPolar ;
		    sParam.GoToPointPolarP.x = approachPoint.x ;
		    sParam.GoToPointPolarP.y = approachPoint.y ;
		    sParam.GoToPointPolarP.finalslope = destAngle ;
		    sParam.GoToPointPolarP.finalVelocity = 0 ;
		    skillSet->executeSkill(sID, sParam);
	        */
			 iState = STEAL ;
			 break;
          
		   }
	   }
	   case SHOOTING :
	   {   
		   if((ballbotdist > 1.5*safeDist)||(state->ballPos.x < state->homePos[botID].x))
		   {
			   iState = APPROACHING ;
			   break ;
		   }

		   float destAngle = Vector2D<int>::angle(state->homePos[botID],state->ballPos) ;
		   sprintf(debug,"SHOOTING \n");
		   Client::debugClient->SendMessages(debug);
		   sID = SkillSet::GoToPoint ;
		   sParam.GoToPointP.x = state->ballPos.x ;
		   sParam.GoToPointP.y = state->ballPos.y ;
		   sParam.GoToPointP.finalslope = destAngle ;
     //	   sParam.GoToPointP.finalVelocity = 0 ;
		   skillSet->executeSkill(sID, sParam);
	       break;
	   }
	   case STEAL :
	   {
		   static bool firstrun = true ;
		   int thresh = 1.5*BOT_BALL_THRESH ;
		   if(firstrun)
		   {
			   if(ballbotdist > 3*BOT_BALL_THRESH)
				   thresh = 2*BOT_BALL_THRESH ;
			   else
				   thresh = BOT_BALL_THRESH ;
			   firstrun = FALSE ;
		   }

		   if((oppballdist < 2.5*BOT_BALL_THRESH)&&(state->ballPos.x > state->homePos[botID].x))
		   {
			   iState = APPROACHING ;
			   break ;
		   }
		   sprintf(debug,"STEALING \n");
		   Client::debugClient->SendMessages(debug);
#ifdef TEST
		       /* 
		       sID = SkillSet::GoToPointPolar ;
			   sprintf(debug,"STEALING , ball : (%d , %d) :: point : (%d , %d) \n",state->ballPos.x,state->ballPos.y,ballPredictPos.x,ballPredictPos.y);
		       Client::debugClient->SendMessages(debug);
			   sParam.GoToPointPolarP.x = approachPoint.x ;
			   sParam.GoToPointPolarP.y = approachPoint.y ;
			   sParam.GoToPointPolarP.finalslope = Vector2D<int>::angle(state->homePos[botID] , state->ballPos ) ;
			   skillSet->executeSkill(sID,sParam);
			   break ;
			   */
#endif

#ifdef GUNJAN 
			     sID = SkillSet::TestSkill;
				 skillSet->executeSkill(sID,sParam);
			     break ;
			 
#endif

#ifdef SPIN_GO
		   if(ballbotdist > thresh)
		   {
		       sprintf(debug,"Stealing & Going \n",firstrun,thresh);
		       Client::debugClient->SendMessages(debug);
			   sID = SkillSet::GoToPointPolar ;
			   sParam.GoToPointPolarP.x = state->ballPos.x;
		       sParam.GoToPointPolarP.y = state->ballPos.y ;
			   skillSet->executeSkill(sID,sParam);
			   break;
		   }
		   else
		   {
			   firstrun = TRUE ;
			   sID = SkillSet::Spin ;
		       sprintf(debug," Steal & Spinnig  \n");
		       Client::debugClient->SendMessages(debug);
			   sParam.SpinP.radPerSec = SGN(state->ballPos.y - state->homePos[botID].y)*MAX_BOT_OMEGA ;
			   skillSet->executeSkill(sID,sParam);
			   break ;
		   }
#endif
	   }
	  }

 
  }; // class TAttackNew
    }; // namespace MyStrategy
}
#endif //  TTCharge_HPP
