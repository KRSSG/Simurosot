#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <math.h>
#include <stdio.h>


#include <list>
#include "comdef.h"


#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"

#include "../common/include/geometry.hpp"
#include "../winDebugger/Client.h"




namespace MyStrategy
{
	
  void SkillSet::testSkill(const SParam &param)
  {
	  char debug[250];

	 float vl,vr,omega;
	 int radius;
	 float vel=sqrt(state->ballVel.x*state->ballVel.x+state->ballVel.y*state->ballVel.y);
	 if(vel<200)
		 radius=300;
	 if(vel<300)
		 radius=400;
	 if(vel<500)
		 radius=800;
	 if(vel<1000)
		 radius=900;
	 if(vel<1200)
		 radius=1000;
	 if(vel<1400)
		 radius=1100;
	 if(vel<1800)
		 radius=1200;
	 else radius=1400;
	 
	 
	 //*************** center calculation **************************
	   /* float dx1,dx2,dy1,dy2,dx,dy;
		std::deque<int> posx,posy;
	    int *p1,*p2;
		std::deque<int>::iterator itx=posx.begin();
		std::deque<int>::iterator ity=posx.begin();

		posx.push_back(state->ballPos.x);
		posy.push_back(state->ballPos.y);
		
		dx1=-*itx+(++(*itx));
		dx2=-*itx+(++(*itx));

		dy1=-*ity+(++(*ity));
		dy2=-*ity+(++(*ity));

		dx=(dx1+dx2)/2;
		dy=(dy1+dy2)/2;

		
		
		if(posx.size()==3 && posy.size()==3)
		{
			posx.pop_front();
			posy.pop_front();
			
			 dest.x=state->ballPos.x+dx*0.00002*state->ballVel.x;
			 dest.y=state->ballPos.y+dy*0.00002*state->ballVel.y;

			 sprintf(debug,"etering if \n");  
			 Client::debugClient->SendMessages(debug);
		}
		else
		{
			dest.x=state->ballPos.x;
			dest.y=state->ballPos.y;

			sprintf(debug,"etering else  \n");  
			Client::debugClient->SendMessages(debug);
		}*/
		
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

				
	 
	 //************************************************************
	 
			if(abs(state->ballPos.y)>HALF_FIELD_MAXY-2*BOT_BALL_THRESH && dest.x<state->ballPos.x && abs(state->homePos[botID].y)>abs(state->ballPos.y))
				{
					dest.y=state->ballPos.y-SGN(state->ballPos.y)*2*BOT_RADIUS;
					dest.x=state->homePos[botID].x;
				}
			
			
			if(abs(state->ballPos.y)>HALF_FIELD_MAXY-1.6*BOT_BALL_THRESH && dest.x<state->ballPos.x && abs(dest.y)>HALF_FIELD_MAXY-2*BOT_BALL_THRESH )
			{
				dest.x=state->ballPos.x+dx*(factorx+0.00001)*state->ballVel.x;
			}
			
			
			
	//*************************************************************
			
			
			
			
			
			
			
			float dif=Vector2D<int>::dist(state->ballPos,state->homePos[botID]);

		  
	    sprintf(debug,"%d %d %d %d %f %f %f %f \n",dest.x,dest.y,state->ballPos.x,state->ballPos.y,vel,state->ballVel.x,state->ballVel.y,dx);  
		Client::debugClient->SendMessages(debug);



		if((dif>2*BOT_BALL_THRESH && state->homePos[botID].x>state->ballPos.x) || (state->homePos[botID].x<state->ballPos.x-0.4*BOT_RADIUS)  )
		{
				if(vel>1800 && state->homePos[botID].x>state->ballPos.x+0.2*BOT_BALL_THRESH && Vector2D<int>::dist(state->homePos[botID],state->ballPos)<1.2*BOT_RADIUS)
			{
				if(state->ballPos.y>state->homePos[botID].y)
					comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
					else
					 comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED);
				return;
			}
			_goToPointPolar(botID,dest,0,atan(-1/tan(Vector2D<int>::angle(state->homePos[botID],state->ballPos))),0,0);
			return;
		}

		else
	{
	    if(state->ballPos.y<state->homePos[botID].y /*&& state->homePos[botID].x<state->ballPos.x*/)
	       {
			     vl=MAX_BOT_SPEED/radius*(radius-BOT_RADIUS);
				 vr=MAX_BOT_SPEED/radius*(radius+BOT_RADIUS);
				 comm->sendCommand(botID,vl,vr);
		   }	
		else
		{
				vl=MAX_BOT_SPEED/radius*(radius-BOT_RADIUS);
				vr=MAX_BOT_SPEED/radius*(radius+BOT_RADIUS);
			    comm->sendCommand(botID,vr,vl);
		}

		if(state->homePos[botID].x<state->ballPos.x+0.2*BOT_BALL_THRESH && Vector2D<int>::dist(state->homePos[botID],state->ballPos)<1.2*BOT_RADIUS)
		{
			if(state->ballPos.y<state->homePos[botID].y)
				comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
				else
				 comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED);
			return;
		}
	 }
	
  }
}
