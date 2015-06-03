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


	
  void execute(const Param& tParam)
    {  float offset = 600;
          float factor = 0.00005;
		  		  int attID=state->ourBotNearestToBall;
		  float ang=Vector2D<int>::angle(state->ballPos, state->homePos[botID]); //attID;  //changed ..............................
		  float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
       int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x - BOT_RADIUS*cos(ang);//1
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y - BOT_RADIUS*sin(ang);  //1

		  int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist - BOT_RADIUS*cos(ang);
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist - BOT_RADIUS*sin(ang);

		 /* if(state->ballPos.x > HALF_FIELD_MAXX/2 && ((state->ballPos.y<-HALF_FIELD_MAXY+2*BOT_BALL_THRESH)||(state->ballPos.y>HALF_FIELD_MAXY-2*BOT_BALL_THRESH)) )
		  {
			  int id =opponentProbableGoalkeeper();
			  targetX= HALF_FIELD_MAXX-GOAL_DEPTH- 3*BOT_RADIUS;
			  if(state->awayPos[id].y<0) targetY= OPP_GOAL_MAXY-BOT_RADIUS;
			  else targetY=OPP_GOAL_MINY+BOT_RADIUS;
		  }*/



	
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
	    


	 // if(targetX<-HALF_FIELD_MAXX+2*GOAL_DEPTH+0.5*BOT_RADIUS && abs(targetY)<OUR_GOAL_MAXY+5*BOT_RADIUS ) targetX=-HALF_FIELD_MAXX+2*GOAL_DEPTH;  

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