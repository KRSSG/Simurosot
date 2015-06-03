#ifndef TRECEIVEBALL_HPP
#define TRECEIVEBALL_HPP
#include <list>
#include "comdef.h"
#include "../Utils/pathPlanners.h"

#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"



#include "../common/include/geometry.hpp"
#include "../winDebugger/Client.h"

//**********************************new one************************
namespace MyStrategy
{
  class TReceiveBall: public Tactic
  {
  public:
    TReceiveBall(const BeliefState* state, int botID) :
    Tactic(Tactic::Stop, state, botID)
    {
     // iState = POSITIONING;
	  iState = APPROACHING;
    } // TReceiveBall
    ~TReceiveBall()
    { } // ~TReceiveBall

   //enum InternalState
   // {
   //         POSITIONING,    // will have different two condition for the ball coming from the corner
   //         APPROACH,
			//CHARGE,
   //         REATTACK
   // }iState;
    enum InternalState
    {
      APPROACHING,
      SPINNING_CCW ,
      SPINNING_CW,
      ATTACKING,
      CLOSE_TO_BALL,
      STUCK
    } iState;            // for the defensive play

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
			//dest.x=0;
			/*dest.y=-1000;
			dest.x=-1000;
			*/
			
			//**************************** gunjan ******************************************
			Vector2D<int> hit;
			Vector2D<int> oppGoal(HALF_FIELD_MAXX,0);
			int n;
			if(state->ballVel.y<600) n=1.3;
			else if(state->ballVel.y<1000) n=1.5;
			else if(state->ballVel.y<1200) n=1.7;
			else if(state->ballVel.y<1400) n=2.2;
			else n=4.6;
			
			// if(((state->ballPos.y)>HALF_FIELD_MAXY - 2*BOT_RADIUS || (state->ballPos.y)< -HALF_FIELD_MAXY + 2*BOT_RADIUS) && state->ballPos.x >0 && state->ballPos.x > HALF_FIELD_MAXX- GOAL_DEPTH-2*BOT_RADIUS )
			//{
			//	//****************** FOR LINGO ********************

			//	dest.x=state->ballPos.x-(2.5)*(state->ballPos.x-(HALF_FIELD_MAXX/2))/(HALF_FIELD_MAXX/2)*BOT_RADIUS; //0.5 as factor  dest.y =-SGN(state->ballPos.y)*HALF_FIELD_MAXY*0.4;//-SGN(state->ballPos.y)*1.5*BOT_RADIUS;
			//	
			//	if(state->ballPos.y<0) dest.y=OPP_GOAL_MAXY-BOT_RADIUS;
			//	else dest.y=OPP_GOAL_MINY+BOT_RADIUS; 


			//	//***********************FOR OUR PREV YEARS CODE*********************
			//	/*int id=0;
			//	for(int i=0;i<4;i++)
			//	{
			//		if(state->awayPos[i].x>HALF_FIELD_MAXX-GOAL_DEPTH-1.5*BOT_RADIUS && abs(state->awayPos[i].y)>OPP_GOAL_MAXY+BOT_BALL_THRESH*0.8)  
			//		{
			//			id=i;
			//			break;
			//		}
			//	}
			//	
			//	dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH;
			//	if(state->awayPos[id].y>0)
			//		dest.y=BOT_RADIUS;
			//	else dest.y=-BOT_RADIUS;*/
			//}
			 if(state->ballPos.x>HALF_FIELD_MAXX-DBOX_WIDTH-GOAL_DEPTH-4*BOT_RADIUS /*&& abs(state->ballPos.y)<OPP_GOAL_MAXY +1*BOT_RADIUS && state->ballVel.x < 1000 && state->ballVel.y<1000*/ && state->ballPos.x > state->homePos[botID].x && (abs(state->ballPos.y)-abs(state->homePos[botID].y))<n*BOT_RADIUS )
			{
						 sID = SkillSet::GoToPoint;
					/////////////////////////////////	
					float factorx = 0.00008;
					  if(state->ballVel.x<200  )
						  factorx=0.00005;
					  else if(state->ballVel.x<1000  )
						  factorx=0.00008;
					  else if(state->ballVel.x<1400)
						  factorx=0.00010;
					  else factorx=0.00014;

					   float factory = 0.00008;
					  if( state->ballVel.y<200 )
						  factory=0.00005;
					  else if(state->ballVel.y<1000 )
						  factory=0.00008;
					  else if(state->ballVel.y<1400)
						  factory=0.00010;
					  else factory=0.00014;
					  //////////////////////////
						float dist;
						dist=Vector2D<int>::dist(state->ballPos,state->homePos[botID]);
						dest.y=state->ballPos.y+ state->ballVel.y*dist*factory;
						dest.x=state->ballPos.x+ state->ballVel.x*dist*factorx-BOT_RADIUS*0.02;
						sParam.GoToPointP.align = false;
						sParam.GoToPointP.x = dest.x;
						sParam.GoToPointP.y = dest.y;
						sParam.GoToPointP.finalslope = Vector2D<int>::angle( Vector2D<int>(OPP_GOAL_X, 0),state->ballPos);
						sParam.GoToPointP.increaseSpeed = 1;

						sParam.GoToPointP.finalVelocity=MAX_BOT_SPEED;
						
						
			}
			 else if(((state->ballPos.y)>HALF_FIELD_MAXY - 1.6*BOT_RADIUS || (state->ballPos.y)< -HALF_FIELD_MAXY + 1.6*BOT_RADIUS) && state->ballVel.x>800) 
			{
								
				dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH-2*BOT_RADIUS;
				dest.y=0;
			 }
			 else if(state->ballPos.x>HALF_FIELD_MAXX-GOAL_DEPTH-2*BOT_RADIUS /*&& abs(state->ballVel.x)<500*/ && abs(state->ballPos.y)>OPP_GOAL_MAXY && abs(state->ballPos.y)<4.5*BOT_RADIUS+OPP_GOAL_MAXY)
			 {
					int id=0;
				for(int i=0;i<4;i++)
				{
					if(state->awayPos[i].x>HALF_FIELD_MAXX-GOAL_DEPTH-1.7*BOT_RADIUS && abs(state->awayPos[i].y)>OPP_GOAL_MAXY+BOT_BALL_THRESH*0.8)  
					{
						id=i;
						break;
					}
				}
				
				dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH;
				if(state->awayPos[id].y>0)
					dest.y=1*BOT_RADIUS;
				else dest.y=-1*BOT_RADIUS; 
				//dest.y=0;

			 }
			else if(state->ballPos.x>0)
			{
				//if(state->ballPos.y<0) dest.y=OPP_GOAL_MAXY-BOT_RADIUS;
				//else dest.y=OPP_GOAL_MINY+BOT_RADIUS;
				int id=0;
				for(int i=0;i<4;i++)
				{
					if(state->awayPos[i].x>HALF_FIELD_MAXX-GOAL_DEPTH-1.5*BOT_RADIUS && abs(state->awayPos[i].y)>OPP_GOAL_MAXY+BOT_BALL_THRESH*1.2)  
					{
						id=i;
						break;
					}
				}
				
				float m;
				m=state->ballVel.y/state->ballVel.x;
				
				/*if(dest.x<HALF_FIELD_MAXX/2)
				dest.y=0;*/
				 
				
					if(state->awayPos[id].y<0) dest.y=OPP_GOAL_MAXY-BOT_RADIUS;
					else dest.y=OPP_GOAL_MINY+BOT_RADIUS;
				

				if(m>0 && state->ballVel.x>0)
				{
					dest.x=state->ballPos.x+abs(state->ballPos.y+SGN(dest.y)*dest.y*-1)/m-BOT_RADIUS*1.9;
				}
				else if(m>0 && state->ballVel.x<0)
				{
					dest.x=state->ballPos.x-abs(state->ballPos.y+SGN(dest.y)*dest.y*-1)/m-BOT_RADIUS*1.4;

				}
				else if(m<0 && state->ballVel.x<0)
				{
					m=-m;
					dest.x=state->ballPos.x-abs(state->ballPos.y+SGN(dest.y)*dest.y*-1)/m-BOT_RADIUS*1.5;
				}
				else if(m<0 && state->ballVel.x>0)
				{
					m=-m;
					dest.x=state->ballPos.x+abs(state->ballPos.y+SGN(dest.y)*dest.y*-1)/m-BOT_RADIUS*1.9;
				}
				//if(dest.x<HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH-5*BOT_RADIUS)
					//dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-DBOX_WIDTH-5*BOT_RADIUS;
				//if(dest.x>HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS) dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS*0.4; 
				
				//if(m)
				////////////////////////////////////////////////////////////////


				


				 if(dest.x<0) 
					{
						
						dest.x=HALF_FIELD_MAXX/2;
					}


					///////////////////////////////////////////////////////////////////////
				
					
					//hit.x=dest.x+3*BOT_RADIUS;
	
			}
			

			/////////////////////////////////////////******************************///////////////////////
			



			 ////////////////////////////////////////*****************************////////////////////////
				
			

			 if(float distance = Vector2D<int>::dist(state->ballPos,state->homePos[botID])< 1.2*BOT_BALL_THRESH && state->ballPos.x > state->homePos[botID].x+BOT_RADIUS*0.1)
		{
			if(state->ballPos.y>state->homePos[botID].y)
				{
							sID = SkillSet::Spin;
							if(FIELD_IS_INVERTED == false)
							sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
						    else
							sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
							skillSet->executeSkill(sID, sParam);
							return;
				}
				else
				 {
							sID = SkillSet::Spin;
							if(FIELD_IS_INVERTED == false)
							sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);
							else
							sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);
             
							skillSet->executeSkill(sID, sParam);
							return;

				 }
		}
		/*if(hit.x>dest.x)
		{
			float x,y;
			x=state->ballVel.x*BOT_RADIUS/MAX_BOT_SPEED;
			y=state->ballVel.y*BOT_RADIUS/MAX_BOT_SPEED;
			float r=sqrt(x*x+y*y);
			if(float distance=Vector2D<int>::dist(state->ballPos,dest)<r)
			{
				sID = SkillSet::GoToPoint;
			    sParam.GoToPointP.y = hit.y;
			    sParam.GoToPointP.x = hit.x;
				//sParam.GoToPointP.finalVelocity=0.4*MAX_BOT_SPEED;   //set acc to you
				sParam.GoToPointP.align = true;
				sParam.GoToPointP.finalslope=Vector2D<int>::angle(oppGoal,state->homePos[botID]);
				skillSet->executeSkill(sID,sParam);
				return;
			}

		}*/


		
		
		/*if(dest.x<-HALF_FIELD_MAXX+2*GOAL_DEPTH+0.5*BOT_RADIUS && abs(dest.y)<OUR_GOAL_MAXY+2*BOT_RADIUS ) 
			dest.x=-HALF_FIELD_MAXX+2*GOAL_DEPTH+0.5*BOT_RADIUS;  
 		if( state->homePos[botID].x <-HALF_FIELD_MAXX+2*GOAL_DEPTH+1.5*BOT_RADIUS && abs(dest.y)<OUR_GOAL_MAXY)
			sParam.GoToPointP.finalVelocity = MAX_BOT_SPEED/3;*/
	
	 if(dest.x>HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS) dest.x=HALF_FIELD_MAXX-GOAL_DEPTH-BOT_RADIUS*0.3; 

	    sID = SkillSet::GoToPoint;
        sParam.GoToPointP.y = dest.y;
        sParam.GoToPointP.x = dest.x;
		   //set acc to you
		sParam.GoToPointP.align = true;
		sParam.GoToPointP.finalslope=Vector2D<int>::angle(oppGoal,state->homePos[botID]);
		skillSet->executeSkill(sID,sParam);
    }

			


      //***************************************************************************


	    



   } ;// class TReceiveBall
} // namespace Strategy
#endif // TRECEIVEBALL_HPP


                                
        
        
       
        
       
      
        
     
    
      

   
    
 







                                
        
        
       
        
       
      
        
     
    
      

   
    
 