#ifndef TTCoverGoal_HPP
#define TTCoverGoal_HPP

#include <list>
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Tactics/tactic.h"
#include "comdef.h"

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
		/*
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
      */
      return 1;//minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
     printf("CoverGoal BotID: %d\n",botID);
	 if(clearing_chance()==false)
		  charge=false;
     if (!isCoverGoalInPosition() && charge==false)
      {
        //Bot elsewhere in field. Bring the bot to the Covergoal position position.
        //  Util::Logger::toStdOut("Bot going to goalkeeper positon.");
        sID = SkillSet::GoToPoint;
        sParam.GoToPointP.align = false;
        sParam.GoToPointP.finalslope =- PI / 2;
        sParam.GoToPointP.x =(-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3);
        sParam.GoToPointP.y = 0;
		  
      }
    else 
	 if(charge==true)
	  { 
		  if(state->ballPos.x <= state->homePos[1].x)
		  {
		    sID = SkillSet::GoToPoint;
			sParam.GoToPointP.align = false;
            sParam.GoToPointP.finalslope =- PI / 2;
		    sParam.GoToPointP.x =(-HALF_FIELD_MAXX + GOAL_DEPTH + 0.6*BOT_RADIUS);  
			sParam.GoToPointP.y = -1*(state->homePos[0].y) + SGN(state->homePos[0].y)*2*BOT_RADIUS; //
			skillSet->executeSkill(sID, sParam);
          }
		  else
          {
			 float ang1;
	        if(abs(state->ballVel.x)<10)
		      ang1 = 0 ;
	        else
	   	      ang1 = atan(state->ballVel.y/state->ballVel.x);
	      sID = SkillSet::GoToPoint;
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.finalslope =- PI / 2;
		  sParam.GoToPointP.x = -HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3; //
          sParam.GoToPointP.y =  state->ballPos.y - (state->ballPos.x - (-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1) ; //tan(ang1)
		  int distance = Vector2D<int>::dist(state->homePos[1],state->ballPos);
		  if(distance < 4*BOT_BALL_THRESH)
		   {
		     sParam.GoToPointP.x = state->ballPos.x;
		     sParam.GoToPointP.y = state->ballPos.y;
		   }
	     }
      }
     else
      {
        if(isPossible()==true)
		 charge=true;
        float dist = Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
        if((state->ballPos.x)<=((state->homePos[botID].x)+100) && dist<3*BOT_BALL_THRESH)
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
          sID = SkillSet::GoToPointGoalie;
          sParam.GoToPointP.x = (-HALF_FIELD_MAXX + GOAL_DEPTH + BOT_RADIUS*3);
          sParam.GoToPointP.y = getBotDestPointY();
          sParam.GoToPointP.align = false;
          sParam.GoToPointP.finalslope = -PI / 2;
      }
      
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
		  
	      return false;
	  
	  }	
		  /*  if ((oppBotBallDist-ourBotBallDist)>2*BOT_RADIUS)
		  {
			  return true;
		  }
		  if(oppBotBallDist>=10*BOT_BALL_THRESH)    //
			  return true ;
        */
	return false;
	}

    float getBotDestPointY()
    {
      Vector2D<int> point;
	  point.y=state->homePos[0].y - SGN(state->homePos[botID].y)*2*BOT_RADIUS;
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
	 /*	
		if(state->ballPos.y < HALF_FIELD_MAXY/2  && state->ballPos.y > -HALF_FIELD_MAXY/2)
		{
			if(abs(state->ballPos.x) < abs(state->homePos[botID].x)  && abs(state->ballPos.x) > 2*HALF_FIELD_MAXX/3)
				return true;
			if((state->ballPos.y) > DBOX_HEIGHT  ||  (state->ballPos.y) < -DBOX_HEIGHT)
				return true;
		}
		return false;
    */
	}
}; // class TDefense
} // namespace Strategy

#endif // TTCharge_HPP