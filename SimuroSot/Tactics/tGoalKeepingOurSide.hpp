#ifndef TGOALKEEPINGOUR_HPP
#define TGOALKEEPINGOUR_HPP

#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../common/include/geometry.hpp"
#include "../Utils/intersection.hpp"
#include "../winDebugger/Client.h"
#include <algorithm>
namespace MyStrategy
{
  class TGoalKeepingOurSide : public Tactic
  {
  public:
     float movementError[10]; // records for previous 10 frames
    float movementErrorSum;
    int movementErrorIndex;
    Point2D<int> prevBotPos;
    float prevBotAngle;
    TGoalKeepingOurSide(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {
            for(int i=0; i<10; i++)
        movementError[i] = 0;
      movementErrorSum  = 0;
      movementErrorIndex = 0;
      prevBotPos = state->homePos[botID];
      prevBotAngle = state->homeAngle[botID];
    } // TGoalKeeping

    ~TGoalKeepingOurSide()
    { } // ~TGoalKeeping
    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 1000000000;
      Point2D<int> goalPos(ForwardX(-(HALF_FIELD_MAXX)), 0);
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
                const int factor = 2;
        float perpend_dist = ForwardX(state->homePos[*it].x - ForwardX(-HALF_FIELD_MAXX+ GOAL_DEPTH + BOT_RADIUS));
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
      
      return 0;//minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
		sID=SkillSet::GoalKeeping;
		skillSet->executeSkill(sID,sParam);


		
/*	
	char debug[50];
     sprintf(debug,"Goalie Bot ID : %d\n",botID);
     //Client::debugClient->SendMessages(debug);
      if (!isGoalKeeperInPosition() )
      {
        sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope =- PI / 2;
        sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH +0.7* BOT_RADIUS);
        sParam.GoToPointP.y = 0;

      }
      else
      {
        //charge
        float dist = Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
     if(dist < 1.2*BOT_BALL_THRESH)
	 {
		 sID = SkillSet::Spin;
		 if(state->ballPos.y>state->homePos[botID].y)
			 sParam.SpinP.radPerSec = MAX_BOT_OMEGA;
		 else
             sParam.SpinP.radPerSec = -MAX_BOT_OMEGA;
		  skillSet->executeSkill(sID, sParam);
		  return;
	 }
       
		if(ForwardX(state->ballPos.x)<=(ForwardX(state->homePos[botID].x)+100) && dist<3*BOT_BALL_THRESH && fabs(state->homeAngle[botID]-PI/2)<0.2)
        {
          int delta = 3*BOT_RADIUS;
          if(state->homePos[botID].y>(OUR_GOAL_MINY) && state->homePos[botID].y<(OUR_GOAL_MAXY))
          {
            sID = SkillSet::Velocity;
            if(state->ballPos.y>state->homePos[botID].y)
            {
              if(state->homeAngle[botID]>0) // angle is +90
              {
                      sParam.VelocityP.vl = 125;
                      sParam.VelocityP.vr = 125;
              }
              else
              {
                      sParam.VelocityP.vl = -125;
                      sParam.VelocityP.vr = -125;
              }
            }
            else
            {
                if(state->homeAngle[botID]>0) // angle is +90
                {
                        sParam.VelocityP.vl = -125;
                        sParam.VelocityP.vr = -125;
                }
                else
                {
                        sParam.VelocityP.vl = +125;
                        sParam.VelocityP.vr = +125;
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
          sParam.GoToPointP.x = ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH +0.7* BOT_RADIUS) ;
          int temp = getBotDestPointY();
          sParam.GoToPointP.y = temp;
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.finalslope = -PI / 2;
		  if(Vector2D<int>::dist(state->homePos[botID],state->ballPos) < 350 ) 
		  {
			  if(state->homePos[botID].y < state->ballPos.y)
			  {
				  sParam.GoToPointP.y = min(OUR_GOAL_MAXY, temp + 350);
			  }
			  else
			  {
				  sParam.GoToPointP.y = max(OUR_GOAL_MINY, temp - 350); 
			  }
		  }
      }
      
      skillSet->executeSkill(sID, sParam);
  */    
      /*
  int vr = 0,vl = 0;
  
  Vector2D<int> Z = state->ballPos;
  int Tx =  Z.x - OPP_GOAL_MAXY;
  int Ty =  Z.y - BOT_RADIUS;
  Vector2D<int> A = state->homePos[botID];
  A.x =  A.x - OUR_GOAL_MAXY;
  A.y = A.y - BOT_RADIUS;
  
  float R = state->homeAngle[botID]*180/PI; 
  if ( R < .001) R = R + 360;
  if( R >360.001) R = R - 360;

  if (A.y > Ty + 90 && A.y > 2700 )
    vr = -100,    vl =-100;
   
  if (A.y < Ty -90 && A.y < 4300 )
    vr = 100,    vl =100;
 
  if (A.y < 2700)
    vr = 100,    vl =100;
  if( A.y > 4300)
    vr = -100,    vl =-100;
  
  if( R > 90.5)
    vl = vl + abs(R - 90);
  else if (R  < 89.5) 
    vr = vr + abs(R - 90);

  sID = SkillSet::Velocity;
  sParam.VelocityP.vl = vl;
  sParam.VelocityP.vr = vr;
  skillSet->executeSkill(sID, sParam);
  */
    }

  /*  bool isGoalKeeperInPosition()
    {
      if ((ForwardX(state->homePos[botID].x) >  (-HALF_FIELD_MAXX + GOAL_DEPTH)) &&
          (ForwardX(state->homePos[botID].x) <= (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*2)) &&
          (state->homePos[botID].y >= OUR_GOAL_MINY - DBOX_HEIGHT) &&
          (state->homePos[botID].y <= (OUR_GOAL_MAXY + DBOX_HEIGHT)))
        return true;
      else
        return false;

    }
    
    int getBotDestPointY()
    {
      Vector2D<int> ballFinalpos, botDestination, point;
      int flag=2;
      
  //    float balldistratio = abs(state->ballPos.x)*1.0/(HALF_FIELD_MAXX-DBOX_WIDTH)<1 ? abs(state->ballPos.x)/(HALF_FIELD_MAXX-DBOX_WIDTH):1 ;
       point.y = state->ballPos.y;
  //    point.y = balldistratio*SGN(state->ballPos.y)*MIN(abs(state->ballPos.y), OUR_GOAL_MAXY); 
  //    
  //    // Workaround for ball velocity 0
  //    if( ( ( fabs(state->ballVel.y) + fabs(state->ballVel.x) < 50) ) || (ForwardX(state->ballVel.x)<0 && ForwardX(state->ballVel.x)>(-50)) )
  //    {
  //     if(ForwardX(state->ballPos.x) > ( -HALF_FIELD_MAXX*0.3))
  //      point.y = 0,flag=0;
  //    }
  //    else if(ForwardX(state->ballVel.x)>0 )
  //    {
  //      if(ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX*0.3))
  //        point.y = 0,flag =0;
  //    }
  //    else if (ForwardX(state->ballVel.x) <=(-50) )
  //    {
  //      if(ForwardX(state->ballPos.x) > (-HALF_FIELD_MAXX*0.8) )
  //      point.y = (state->ballVel.y/state->ballVel.x)*(ForwardX(-HALF_FIELD_MAXX+ GOAL_DEPTH + BOT_RADIUS*1.5) - (state->ballPos.x)) + state->ballPos.y,flag = 1;
  //    }
  //      
  //  //   Set Limits on y to not exceed DBOX Y Limits
  //    if(point.y < OUR_GOAL_MINY)
  //     {
  //    //   if(point.y >-HALF_FIELD_MAXY)
  //       point.y = OUR_GOAL_MINY + 2*BOT_RADIUS;
  //   //   else
  //    //    point.y = 0;
  //     }
  //     
  //    else if(point.y > OUR_GOAL_MAXY)
  //    {
  ////       if(point.y < HALF_FIELD_MAXY)
  //        point.y = OUR_GOAL_MAXY - 2*BOT_RADIUS;
  ////        else
  ////        point.y = 0;
  //     }

          
       if(point.y < OUR_GOAL_MINY-BOT_RADIUS)
       {
    //     if(point.y >-HALF_FIELD_MAXY)
         point.y = OUR_GOAL_MINY-BOT_RADIUS;
   //     else
      //    point.y = 0;
       }
       
      else if(point.y > OUR_GOAL_MAXY+ BOT_RADIUS)
      {
        // if(point.y < HALF_FIELD_MAXY)
          point.y = OUR_GOAL_MAXY + BOT_RADIUS;
       //   else
      //    point.y = 0;
       }


  
          
      return point.y;
    }
	*/
  };// class TGoalKeepingOurside
} // namespace Strategy

#endif // TGOALKEEPINGOUR_HPP