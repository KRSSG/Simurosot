#ifndef TMID_HPP
#define TMID_HPP
 
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
  class TMidField : public Tactic
  {
    static const int offset = 400;

	// Corner Case: wall, ball, bot in line
    float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
  public:
    TMidField(const BeliefState* state, int botID) :
      Tactic(Tactic::MidField, state, botID)
    {
      iState = DEFENDING;
	  iState2 = POSITION;
      for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
      hasAchievedOffset = 0;
    } // TAttack

    ~TMidField()
    { } // ~TAttack
    enum InternalState
    {
      DEFENDING,
      MIDFIELDING,
      ATTACKING,
    } iState;

	enum InternalState2
    {
      POSITION,
      CLOSE_TO_BALL,
      SPINNING_CCW,
      SPINNING_CW
    } iState2;

    int hasAchievedOffset;
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      
      return minv;

    } // chooseBestBot

	void getbotdest(Param tParam,float *x,float *y,float *slope)
    {
		Vector2D<int>p1(0,HALF_FIELD_MAXY);
      Vector2D<int>p2(0,-HALF_FIELD_MAXY);
      Vector2D<int>b(state->ballPos.x,state->ballPos.y);
      float m;
      
      //calculating x , y and slope required
      if(p2.x==p1.x) 
      {
        *x = p1.x;
        *y = state->ballPos.y;
        *slope = PI/2;
      }
      else if(p2.y==p1.y)
      {
        *x = state->ballPos.x;
        *y = p2.y;
        *slope = 0;
      }
      else
      { 
        m = (p2.y-p1.y)/(p2.x-p1.x);      
        *x = ( m*(b.y-p1.y) + m*m*p1.x + b.x)/(1 + m*m);
        *y = m*(*x - p1.x) + p1.y;
        *slope = atan(m);
      }
      
      Vector2D<float>p(*x,*y);

      //setting limits on x and y
      if((*x-p1.x)*(*x-p2.x)>0 || (*y-p1.y)*(*y-p2.y)>0)
      {
        if(abs(*x-p1.x)>abs(*x-p2.x) || abs(*y-p1.y)>abs(*y-p2.y))
          *x = p2.x,*y = p2.y;
        else *x = p1.x,*y = p1.y;
      }
    }

    void execute(const Param& tParam)
    {
      float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
   //   prevBotPos = state->homePos[botID];
   //   prevBotAngle = state->homeAngle[botID];]

      switch(iState)
	  {
        
        case ATTACKING:
         {
          /* Ball is in our quadrant and with the enemy bot */

			 //write here for converting to DEFENDING

			 if(ForwardX(state->ballPos.x) < 0)
			{
            iState = DEFENDING ;
            break;
            }

			 float distance = Vector2D<int>::dist(state->ballPos,state->homePos[state->ourBotNearestToBall]);
			 if(distance < 3*BOT_BALL_THRESH && ForwardX(state->ballPos.x) > 0)
			 {
				 iState = MIDFIELDING;
				 break;
			 }
			 
			 //additional condition that there is only bot in D may be added
			 if(ForwardX(state->homePos[botID].x) > HALF_FIELD_MAXX - DBOX_WIDTH && abs(state->homePos[botID].y) < OUR_GOAL_MAXY){
				 iState = MIDFIELDING;
				 break;
			 }

			 cout << "ATTACKING\n";
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));

                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
          float factor = 0.00005;
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
          int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.


          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {

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

		sParam.GoToPointP.align = false;
        
		if(sParam.GoToPointP.x < -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS)   //acc to rules...........................
				sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS;

		if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) sParam.GoToPointP.align = true;
          skillSet->executeSkill(sID, sParam);
          
        break;
         }

        case DEFENDING:
        {
			
			dist = Vector2D<int>::dist(state->ballPos,state->homePos[state->ourBotNearestToBall]);
			if(dist > 3*BOT_BALL_THRESH && abs(state->ballPos.y) < OUR_GOAL_MAXY)
			{
				iState = ATTACKING;
				break;
			}

			if(ForwardX(state->ballPos.x) > 0)
			{
				iState = MIDFIELDING;
				break;
			}

			cout << " DEFENDING\n";

          /* Ball is with opp bot who is charging towards goal. So defend the goal */

          
			Vector2D<int> goaliemid(ForwardX(-HALF_FIELD_MAXX),0);
			sID = SkillSet::DefendPoint;
			sParam.DefendPointP.x = ForwardX(-HALF_FIELD_MAXX +  GOAL_DEPTH);
			sParam.DefendPointP.y = 0;
			sParam.DefendPointP.finalslope = Vector2D<int>::angle(state->ballPos,goaliemid);

			/*float factor=0.0005;
			float oppBotDist=Vector2D<int>::dist(state->homePos[botID], state->awayPos[state->oppBotNearestToBall]);
			sID = SkillSet::GoToPoint;
			sParam.GoToPointP.x = state->awayPos[state->oppBotNearestToBall].x + (int)(oppBotDist*factor*state->awayVel[state->oppBotNearestToBall].x);
			sParam.GoToPointP.y = state->awayPos[state->oppBotNearestToBall].y + (int)(oppBotDist*factor*state->awayVel[state->oppBotNearestToBall].y);
			sParam.GoToPointP.finalslope = Vector2D<int>::angle(state->ballPos,goaliemid);
			sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;*/
			if(sParam.GoToPointP.x < -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS)   //acc to rules.............................
				sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS;
			skillSet->executeSkill(sID,sParam);

          break;
        }

		case MIDFIELDING:
			{
			//defendline

			//and a switching condition to defend
			if(ForwardX(state->ballPos.x) <0){

				iState = DEFENDING;
				break;
			}

			float distance = Vector2D<int>::dist(state->ballPos,state->homePos[state->ourBotNearestToBall]);
			float distance2 = Vector2D<int>::dist(state->ballPos,state->awayPos[state->oppBotNearestToBall]);
			if(state->oppBotNearestToBall != botID){
				if(ForwardX(state->ballPos.x) >0 && distance2 > 3*BOT_BALL_THRESH && distance > 3*BOT_BALL_THRESH)
				{
					iState = ATTACKING;
					break;
				}
			}
			else
			{
				if(ForwardX(state->ballPos.x) >0 && distance2 > 3*BOT_BALL_THRESH)
				{
					iState = ATTACKING;
					break;
				}
			}


			cout <<" MIDFIELDING\n";

			
            sID = SkillSet::GoToPoint;
			  getbotdest(tParam,&sParam.GoToPointP.x,&sParam.GoToPointP.y,&sParam.GoToPointP.finalslope);
            sParam.GoToPointP.align = false;
			sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;
			//skillSet->executeSkill(sID,sParam);
			//return;
            //if( Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < BOT_BALL_THRESH*1.1 )
              //iState2 = CLOSE_TO_BALL;
			if(sParam.GoToPointP.x < -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS)   //acc to rules .....................
				sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS;
         skillSet->executeSkill(sID, sParam);

			break;
		}

		break;
      }

	  }
 
    }; // class TMidField
     
}// namespace MyStrategy
#endif // TMID_HPP
