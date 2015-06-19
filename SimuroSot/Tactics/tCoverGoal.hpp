#ifndef TTCoverGoal_HPP
#define TTCoverGoal_HPP
//#define abcd
#include <list>


#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Tactics/tactic.h"
#include "comdef.h"
#include "../Utils/pathPlanners.h"

//#include <list>
//#include "comdef.h"
//#include "../Tactics/tactic.h"
//#include "../Skills/skillSet.h"
//#include "../Core/beliefState.h"
//#include "../common/include/config.h"
#include "../common/include/geometry.hpp"
#include "../Utils/intersection.hpp"
#include "../winDebugger/Client.h"
#define ANGLE_TO_DIST 0
#define MIN(a,b) (a) < (b) ? (a) : (b)



#define DEBUG 1
namespace MyStrategy{
	
class TCoverGoal : public Tactic{
public:
    int state1;
     float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
	bool charge;
    float prevBotAngle;
    TCoverGoal(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
		charge=false;
      state1 = 0;
            for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
    } // TGoalKeeping

    ~TCoverGoal()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 1000000000;
      Point2D<int> goalPos((-(HALF_FIELD_MAXX)), 0);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
const int factor = 0.2;
        float perpend_dist =(state->homePos[*it].x - (-HALF_FIELD_MAXX));//state->home_goalpoints[2] is center of our goal
Vector2D<int> goal;
goal.x = OUR_GOAL_X;
goal.y = 0;
float dist_from_goal = Vector2D<int>::dist(state->homePos[*it], goal);
if(dist_from_goal + factor * perpend_dist < mindis)
        {
          mindis = dist_from_goal + factor * perpend_dist;
          minv = *it;
        }
      }

      return 1;//minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
		float ang1;

		static int ishaan=0;
		int hasAchievedOffset = 0; 
		Vector2D<int> Point,point2;
		Point.x=state->homePos[0].x;
		Point.y=state->homePos[0].y;
		//float veL=state->awayVel[state->oppBotNearestToBall];
		float diS = Vector2D<int>::dist(state->awayPos[state->oppBotNearestToBall],state->ballPos);
		float diS2=Vector2D<int>::dist(state->homePos[botID],state->ballPos);

		printf("CoverGoal BotID: %d\n",botID);
		///THE GIVEN STATEMENT IS TO AVOID BEING DRAGGED INTO THE OTHER HALF 
		//IT WILL START SPINNING IF IT IS IN THE OTHER HALF 
		/*
		if(state->homePos[botID].x>0)
		{
			
			
			 if(ishaan<5 &&  ishaan>=0)
			{
			 sID = SkillSet::Velocity;
			 sParam.VelocityP.vl = +120;
             sParam.VelocityP.vr = -120;
			 skillSet->executeSkill(sID, sParam);
			 ishaan++;
			// if(ishaan==5)ishaan=-200;
			 return ; 
			 }
			 ishaan++;
			 
			 
			//comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
		   	
		}
		*/

	  if(clearing_chance()==false)
		  charge=false;

	  if (charge==false  && clearing_chance()==false)
      {
        if(state->ballPos.x > HALF_FIELD_MAXX - DBOX_WIDTH -BOT_RADIUS )

		  {
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope =0;
		sParam.GoToPointP.x =-HALF_FIELD_MAXX/2+2*BOT_BALL_THRESH;
        sParam.GoToPointP.y =state->ballPos.y ;
		  }
		else if(state->ballPos.x < HALF_FIELD_MAXX - DBOX_WIDTH -BOT_RADIUS )
		{
			 
			sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope =0;
        //0.45*(state->ballPos.x -(HALF_FIELD_MAXX - GOAL_DEPTH ));
        sParam.GoToPointP.x =0.3*(state->ballPos.x +(HALF_FIELD_MAXX - GOAL_DEPTH ))-(HALF_FIELD_MAXX - GOAL_DEPTH );
		if(state->ballVel.x< 0 && abs(state->ballVel.x)>50 )
		{
			
			ang1 = atan(state->ballVel.y/state->ballVel.x);
		    sParam.GoToPointP.y = state->ballPos.y - ((state->ballPos.x) - (-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1) ;
			
			 if(sParam.GoToPointP.y >=  HALF_FIELD_MAXY)
		     sParam.GoToPointP.y =  state->ballPos.y;
	         if(sParam.GoToPointP.y <= -HALF_FIELD_MAXY)
		     sParam.GoToPointP.y =  state->ballPos.y;

		}
		else
        sParam.GoToPointP.y = state->ballPos.y;


		}
      }
	  else if(diS2<2*BOT_BALL_THRESH && state->ballPos.x<0)// do not go in goalie region daalna hai abhi
	  {
		  //ishaan=0;

		  /// SHOOT FEATURE
		
		//sID = SkillSet::GoToPoint;
		//sParam.GoToPointP.align = false;
		//sParam.GoToPointP.finalslope = 0;
		//sParam.GoToPointP.x = state->ballPos.x; 
		//sParam.GoToPointP.y = state->ballPos.y; 
		//if(state->ballPos.x>state->homePos[botID].x) 
		//{
		//	sParam.GoToPointP.finalVelocity=75;
		//	/* if(state->ballPos.y>state->homePos[botID].y)
		//		comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
		//		else
		//		 comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED);*/
		//}
		
		  sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
                 // sprintf(debug,"ballgoaldist = %f\n",ballgoaldist);
                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
          float factor = 0.00005;
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
          int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.
               //   sprintf(debug,"i am here 2\n");
                        //Client::debugClient->SendMessages(debug);
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
           //               sprintf(debug,"i am here 3\n");
                        //Client::debugClient->SendMessages(debug);
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
         
         // sprintf(debug,"i am here 5\n");
                  ////Client::debugClient->SendMessages(debug);
          //sprintf(debug,"x3 = %d y3 = %d ballVel x = %f y = %f\n",x3,y3,state->ballVel.x,state->ballVel.x,state->ballVel.y);
          //Client::debugClient->SendMessages(debug);
          Vector2D<int> offsetpt(x3, y3);
          int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
          if(dist2 < 300)
            hasAchievedOffset = 1;
                  /* Bot is already between ball and offset point */
                  //else if(Vector2D<int>::dist(offsetpt,state->ballPos) < 500 + Vector2D<int>::dist(state->homePos[botID],state->ballPos) + dist2) hasAchievedOffset = 1; 
          else if(diS2 > 2 * offset)
            hasAchievedOffset = 0; 

          if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x)) 
            hasAchievedOffset = 0;
            
          sParam.GoToPointP.x = x3;
          sParam.GoToPointP.y = y3;
                //  sprintf(debug,"x3 = %d y3 = %d\n",x3,y3);
                  ////Client::debugClient->SendMessages(debug);
         // sprintf(debug,"i am here 6\n");
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

			//sprintf(debug,"Going to point %f %f\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
			//Client::debugClient->SendMessages(debug);
			sParam.GoToPointP.align = false;
      if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) 
		  sParam.GoToPointP.align = true;
		  //READ COMMENTS BELOW AND PUT THEM HERE ACCORDINGLY

		  }

	  else if(state->ballPos.x<0)
	    {
	        //Vector2D<int> P2;
			//ishaan=0;
             sID = SkillSet::GoToPoint;
		  if(state->ballPos.x <= 0 && state->ballPos.x >= state ->homePos[botID].x && state->ballPos.x>=-(HALF_FIELD_MAXX-1.5*DBOX_WIDTH))
		  {

				if( state->ballVel.x>1000)
				{
					sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;

					sParam.GoToPointP.x = -HALF_FIELD_MAXX + 1.2*DBOX_WIDTH	; // ..................changed
					ang1 = atan(state->ballVel.y/state->ballVel.x);
				    sParam.GoToPointP.y = state->ballPos.y - ((state->ballPos.x) - (-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1) ;
			
					if(sParam.GoToPointP.y >=  HALF_FIELD_MAXY)
					sParam.GoToPointP.y =  state->ballPos.y;
					if(sParam.GoToPointP.y <= -HALF_FIELD_MAXY)
					sParam.GoToPointP.y =  state->ballPos.y;

					
					///YAHA PAR DAAL ATTACK KA FEATURE EK FINAL VELOCITY SE BHEJ DE AUR UPAR EK ELSE IF CONDITION HAI else if(diS2<2*BOT_BALL_THRESH) USMAIN ABHI CHANGE KARKE MUJHE FATAFUT BHEJ 
	////YAHA PAR GOTOPOINT WITH VELOCITY DAALDE AUR UPAR ELSE IF MAIN ATTACK AUR SPIN SAB KUCH DAAL DE WHEN BALL IS TO THE LEFT OF THE BOT
				}
				else if(abs(diS-diS2)<4*BOT_RADIUS)
				{
					sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
					sParam.GoToPointP.x =  state->ballPos.x;
					sParam.GoToPointP.y =  state->ballPos.y;
					sParam.GoToPointP.finalVelocity=100;

				}
            else if(diS>diS2*1.5)
                {
                    sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
					sParam.GoToPointP.x =  state->ballPos.x;
					sParam.GoToPointP.y =  state->ballPos.y;
					sParam.GoToPointP.finalVelocity=100;
					////YAHA PAR BHI GOTO POINT WITH VELOCITY DAAL 

                }
                else
                {
                    /*sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
                    sParam.GoToPointP.x = -HALF_FIELD_MAXX + 1.2*DBOX_WIDTH	; // ..................changed
					sParam.GoToPointP.y = state->ballPos.y; 
					*/
					//

					sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
                 // sprintf(debug,"ballgoaldist = %f\n",ballgoaldist);
                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
          float factor = 0.00005;
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
          int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.
               //   sprintf(debug,"i am here 2\n");
                        //Client::debugClient->SendMessages(debug);
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
           //               sprintf(debug,"i am here 3\n");
                        //Client::debugClient->SendMessages(debug);
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
         
         // sprintf(debug,"i am here 5\n");
                  ////Client::debugClient->SendMessages(debug);
          //sprintf(debug,"x3 = %d y3 = %d ballVel x = %f y = %f\n",x3,y3,state->ballVel.x,state->ballVel.x,state->ballVel.y);
          //Client::debugClient->SendMessages(debug);
          Vector2D<int> offsetpt(x3, y3);
          int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
          if(dist2 < 300)
            hasAchievedOffset = 1;
                  /* Bot is already between ball and offset point */
                  //else if(Vector2D<int>::dist(offsetpt,state->ballPos) < 500 + Vector2D<int>::dist(state->homePos[botID],state->ballPos) + dist2) hasAchievedOffset = 1; 
          else if(diS2 > 2 * offset)
            hasAchievedOffset = 0; 

          if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x)) 
            hasAchievedOffset = 0;
            
          sParam.GoToPointP.x = x3;
          sParam.GoToPointP.y = y3;
                //  sprintf(debug,"x3 = %d y3 = %d\n",x3,y3);
                  ////Client::debugClient->SendMessages(debug);
         // sprintf(debug,"i am here 6\n");
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

			//sprintf(debug,"Going to point %f %f\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
			//Client::debugClient->SendMessages(debug);
			sParam.GoToPointP.align = false;
      if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) 
		  sParam.GoToPointP.align = true;

					//
                }
            
 //return;
            //skillSet->executeSkill(sID, sParam);
          }
		  else if (state->ballPos.x <= 0 && state->ballPos.x <= state ->homePos[botID].x && state->ballPos.x>=-(HALF_FIELD_MAXX-1.2*DBOX_WIDTH))
		  {
           //
			   sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = true;
          float ballgoaldist = Vector2D<int>::dist(state->ballPos, Vector2D<int>(OPP_GOAL_X, 0));
                 // sprintf(debug,"ballgoaldist = %f\n",ballgoaldist);
                  ////Client::debugClient->SendMessages(debug);
          float offset = 600;
          float factor = 0.00005;
          int ballBotDist = (int)Vector2D<int>::dist(state->homePos[botID],state->ballPos);
          int targetX = state->ballPos.x + (int)ballBotDist * factor * state->ballVel.x;
          int targetY = state->ballPos.y + (int)ballBotDist * factor * state->ballVel.y;
          int x3 = (targetX * (ballgoaldist + offset) - offset * OPP_GOAL_X) / ballgoaldist;
          int y3 = (targetY * (ballgoaldist + offset)) / ballgoaldist;
                  
          /// logarithmic search to place offset point in field.
               //   sprintf(debug,"i am here 2\n");
                        //Client::debugClient->SendMessages(debug);
          while(!LocalAvoidance::isPointInField(Point2D<int>(x3, y3))) 
          {
           //               sprintf(debug,"i am here 3\n");
                        //Client::debugClient->SendMessages(debug);
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
         
         // sprintf(debug,"i am here 5\n");
                  ////Client::debugClient->SendMessages(debug);
          //sprintf(debug,"x3 = %d y3 = %d ballVel x = %f y = %f\n",x3,y3,state->ballVel.x,state->ballVel.x,state->ballVel.y);
          //Client::debugClient->SendMessages(debug);
          Vector2D<int> offsetpt(x3, y3);
          int dist2 = Vector2D<int>::dist(offsetpt, state->homePos[botID]);
          if(dist2 < 300)
            hasAchievedOffset = 1;
                  /* Bot is already between ball and offset point */
                  //else if(Vector2D<int>::dist(offsetpt,state->ballPos) < 500 + Vector2D<int>::dist(state->homePos[botID],state->ballPos) + dist2) hasAchievedOffset = 1; 
          else if(diS2 > 2 * offset)
            hasAchievedOffset = 0; 

          if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x)) 
            hasAchievedOffset = 0;
            
          sParam.GoToPointP.x = x3;
          sParam.GoToPointP.y = y3;
                //  sprintf(debug,"x3 = %d y3 = %d\n",x3,y3);
                  ////Client::debugClient->SendMessages(debug);
         // sprintf(debug,"i am here 6\n");
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

			//sprintf(debug,"Going to point %f %f\n",sParam.GoToPointP.x,sParam.GoToPointP.y);
			//Client::debugClient->SendMessages(debug);
			sParam.GoToPointP.align = false;
      if(ForwardX(state->ballPos.x) < ForwardX(state->homePos[botID].x) && Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < 1000) 
		  sParam.GoToPointP.align = true;

			//
		  }
            else 
            {
                	if(state->ballPos.x<=-HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS)
			{
				if(state->ballPos.y>=OUR_GOAL_MAXY + 1.8*BOT_RADIUS)
				{
					sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
			
					sParam.GoToPointP.x =state->ballPos.x	; // ..................changed
					sParam.GoToPointP.y = state->ballPos.y; //................changed
					if(state->ballPos.x>state->homePos[botID].x) 
					{
						sParam.GoToPointP.finalVelocity=25;
						/* if(state->ballPos.y>state->homePos[botID].y)
								comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
							else
								comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED);*/
					}
					////YAHA PAR BHI GOTO POINT WITH VELOCITY DAAL 
	
				}
				else if(state->ballPos.y<=OUR_GOAL_MINY - 1.8*BOT_RADIUS)
				{
					
					sParam.GoToPointP.align = false;
					sParam.GoToPointP.finalslope = 0;
			
					sParam.GoToPointP.x =state->ballPos.x	; // ..................changed
					sParam.GoToPointP.y = state->ballPos.y; //................changed
					if(state->ballPos.x>state->homePos[botID].x) 
					{
						sParam.GoToPointP.finalVelocity=25;
						/* if(state->ballPos.y>state->homePos[botID].y)
								comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
							else
								comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED);*/
					}
					////YAHA PAR BHI GOTO POINT WITH VELOCITY DAAL 

				}

            }
	  }

	    }

		// previous code is written onwawrds - never executes further 
      else
      {
        if(isPossible()==true)
		 charge=true;

         float diS2 = Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
        if((state->ballPos.x)<=((state->homePos[botID].x)+100) && diS2<3*BOT_BALL_THRESH)
        {
          int delta = BOT_RADIUS;
          if(state->homePos[botID].y>(OUR_GOAL_MINY-delta) && state->homePos[botID].y<(OUR_GOAL_MAXY+delta))
          {
            sID = SkillSet::Velocity;
            if(state->ballPos.y>state->homePos[botID].y)
            {
              if(state->homeAngle[botID]>0) // angle is +90
              {
                      sParam.VelocityP.vl = 120;
                      sParam.VelocityP.vr = 120;
              }
              else
              {
                      sParam.VelocityP.vl = -120;
                      sParam.VelocityP.vr = -120;
              }
            }
            else
            {
                if(state->homeAngle[botID]>0) // angle is +90
                {
                        sParam.VelocityP.vl = -120;
                        sParam.VelocityP.vr = -120;
                }
                else
                {
                        sParam.VelocityP.vl = +120;
                        sParam.VelocityP.vr = +120;
                }
            }

           char data[250];
           wsprintf(data,"Velocity l,r = %f , %f \n",sParam.VelocityP.vl,sParam.VelocityP.vr);
           //Client::debugClient->SendMessages(data);

           skillSet->executeSkill(sID, sParam);
           return;
         }
       }
                  //will not reach here if above code is executed
          sID = SkillSet::GoToPoint;
          sParam.GoToPointP.x = (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3);
          sParam.GoToPointP.y = getBotDestPointY();
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.finalslope = -PI / 2;
      }

	  
			////////////////////////////////////////////////////
			
			
			if (state->homePos[0].x + BOT_RADIUS > state->homePos[botID].x)		//Defender will not disturb GoalKeeper from the back: By KD
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
				
						}


			//////////////////////////////////////////////////////
 if(sParam.GoToPointP.x < -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS && abs(state->homePos[botID].y)<OUR_GOAL_MAXY+4*BOT_RADIUS)      // acc to rules....................
				sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS;
		

			skillSet->executeSkill(sID, sParam);
    }

    bool isCoverGoalInPosition()
    {
      //Util::Logger::toStdOut("Half_field_max : %d, Dbox_width : %d\n",HALF_FIELD_MAXX,DBOX_WIDTH);
      if (((state->homePos[botID].x) >  (-HALF_FIELD_MAXX + GOAL_DEPTH + 2*BOT_RADIUS)) &&
          ((state->homePos[botID].x) <= (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*4)) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY - DBOX_WIDTH) &&
          (state->homePos[botID].y <= (OUR_GOAL_MAXY + DBOX_WIDTH)))
        return true;
      else
        return false;

    }
    bool isPossible()
	{
	  float oppBotBallDist=Vector2D<int>::dist(state->ballPos,state->awayPos[state->oppBotNearestToBall]);
	  float ourBotBallDist=Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
	  if(clearing_chance()==true)
	  {
		  if((ourBotBallDist >= 10*BOT_RADIUS))//||(ourBotBallDist < 1.5*BOT_BALL_THRESH))
			  return true;
		  //shouldn't it be the opoposite case
	      return false;

	  }
		 
	return false;
	}

    float getBotDestPointY()//what is the reason behind taking these values of point.y
    {
      Vector2D<int> point;//a change has been made 0 put in place of botID
	  point.y=state->homePos[0].y - SGN(state->homePos[0].y)*2*BOT_RADIUS;        //maintains a certain distace from the goalie
	  /*if(abs(state->ballPos.y) < DBOX_HEIGHT)
	 	        point.y=state->ballPos.y;*/
	  if(abs(state->ballPos.x)>abs(state->homePos[botID].x) && SGN(state->ballPos.x)==SGN(state->homePos[botID].x) && abs(state->ballPos.y)<=DBOX_WIDTH)
		  point.y=state->ballPos.y - SGN(state->homePos[botID].y)*4*BOT_RADIUS;
      return (float)point.y;
    }
    bool clearing_chance()
    {

		//if((state->ballPos.x >= state->homePos[1].x)&&(state->ballPos.x < 0))// 0.1*HALF_FIELD_MAXX))
          if(state->ballPos.x < 0)
			  return true;

		return false;
	
	}
}; // class TDefense
} // namespace Strategy

#endif // TTCharge_HPP