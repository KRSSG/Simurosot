#ifndef TRECEIVEBALL_HPP
#define TRECEIVEBALL_HPP
#include <list>
#include "comdef.h"
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"

//**********************************new one************************
namespace MyStrategy
{
  class TReceiveBall: public Tactic
  {
  public:
    TReceiveBall(const BeliefState* state, int botID) :
    Tactic(Tactic::Stop, state, botID)
    {
      iState = POSITIONING;
    } // TReceiveBall
    ~TReceiveBall()
    { } // ~TReceiveBall

   enum InternalState
    {
            POSITIONING,    // will have different two condition for the ball coming from the corner
            APPROACH,
			CHARGE,
            REATTACK
    }iState;

   inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      float minv = *(freeBots.begin());
      float mindis = 1e12;
      const Vector2D<int> &ballpos = state->ballPos;
      const Vector2D<float> &ballvel = state->ballVel;
      if(state->ballVel.absSq() < ZERO_VELOCITY_THRESHOLD_SQ) {
      // For stopped ball, take the bot which is nearest to the ball
        for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
        {
          const Vector2D<int> &botpos = state->homePos[*it];
          
          int dist = abs(state->homePos[*it].y + SGN(state->ballPos.y)*(HALF_FIELD_MAXY-2*BOT_RADIUS)/2);
          if(dist  < mindis)
          {
            mindis =  dist;
            minv = *it;
          }
        }
      } 
	  else {
        // For moving ball take the bot which is nearest to the ray of ball direction.
        for(std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
        {
          const Vector2D<int> &botpos = state->homePos[*it];
          float dis_from_ball_line = abs((botpos.y - ballpos.y)/ballvel.y - (botpos.x - ballpos.x)/ballvel.x) + 
                                      Vector2D<int>::dist(ballpos, botpos)/10;
          if(state->ballVel.x * (botpos.x - ballpos.x) < 0)
            dis_from_ball_line += 1e10;
          if(dis_from_ball_line < mindis)
          {
            mindis = dis_from_ball_line;
            minv = *it;
          }
        }
      }
     int dis3 , dis4;
	   dis3 = Vector2D<int>::dist(state->homePos[3],state->ballPos);
	   dis4 = Vector2D<int>::dist(state->homePos[4],state->ballPos);
	   if(dis4>dis3)
		   return 4;

	   else 
		   return 3;
      return 4;//minv;
     } // chooseBestBot  :: will controlled by the tester function
void execute(const Param& tParam)
	 {
	  char debug[50];
	  Vector2D<int> dest;
	//  if(state->ballPos.x>-HALF_FIELD_MAXX/2)
	//	{
			dest.x=state->ballPos.x-(2.5)*(state->ballPos.x-(HALF_FIELD_MAXX/2))/(HALF_FIELD_MAXX/2)*BOT_RADIUS; //0.5 as factor  dest.y =-SGN(state->ballPos.y)*HALF_FIELD_MAXY*0.4;//-SGN(state->ballPos.y)*1.5*BOT_RADIUS;
			dest.y=SGN(state->ballPos.y)*0.4*HALF_FIELD_MAXY*(-1);
			 
			if(state->ballPos.x > (HALF_FIELD_MAXX - GOAL_DEPTH - 0.7*BOT_RADIUS - 2*BOT_RADIUS))
                 {  dest.x = state->ballPos.x;
			       dest.y = -1*state->ballPos.y;
			     }
	    Vector2D<int> oppGoal(HALF_FIELD_MAXX,0);
		if(dest.x < (-HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS))  //changed acc to rules ........................
	    {
		  dest.x = -HALF_FIELD_MAXX + GOAL_DEPTH + DBOX_WIDTH + BOT_RADIUS;
	    }
		//change the comment acc to the rules 
	  /*
		if((dest.x >( HALF_FIELD_MAXX - GOAL_DEPTH - 2*BOT_RADIUS))&&(abs(dest.y) < OPP_GOAL_MAXY - 0.5*BOT_RADIUS))  //changed acc to rules.................
		  {
			  dest.y = SGN(dest.y)*(OUR_GOAL_MAXY - 0.5*BOT_RADIUS) ;
			  dest.x =  HALF_FIELD_MAXX -GOAL_DEPTH - 2*BOT_RADIUS ;
		  } 
		*/
	    sID = SkillSet::GoToPoint;
        sParam.GoToPointP.y = dest.y;
        sParam.GoToPointP.x = dest.x;
		//sParam.GoToPointP.finalVelocity=0.4*MAX_BOT_SPEED;   //set acc to you
		sParam.GoToPointP.align = true;
		sParam.GoToPointP.finalslope=Vector2D<int>::angle(oppGoal,state->homePos[botID]);
		skillSet->executeSkill(sID,sParam);
    }



   } ;// class TReceiveBall
} // namespace Strategy
#endif // TRECEIVEBALL_HPP


                                
        
        
       
        
       
      
        
     
    
      

   
    
 







                                
        
        
       
        
       
      
        
     
    
      

   
    
 