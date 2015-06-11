#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include <math.h>
#include <stdio.h>
#include "comdef.h"
#include "../winDebugger/Client.h"
#include <fstream>

namespace MyStrategy
{
	


	void SkillSet::kickBall(const SParam &param)
  {
// is called only when bot is behind the ball  
	  char debug[250];
	//  if (abs(state->ballPos.x)>HALF_FIELD_MAXX ||abs(state->ballPos.y)>HALF_FIELD_MAXY || abs(state->ballVel.x)>10000 || abs(state->ballVel.y)>10000) return;
	 // written for safe condition 
	   if(state->ballPos.x <= state->homePos[botID].x) 
	   {
		   comm->sendCommand(botID,0,0) ;
		   return ;
	   }
	   // if there is kicking situation
	   float d = (HALF_FIELD_MAXX-GOAL_DEPTH)/10 ;
	   float m ;
	   if(abs(state->ballVel.x)<10)
		   m = INF ;
	   else
		   m = state->ballVel.y/state->ballVel.x;
	   
	   float xpos[11]={0},ypos[11]={0};
	   float time_error[11]={0};
	   int minimum = INF , vbot=60;     // vbot is the average bot speed .. can be set to something else
	   Vector2D<int> dest;
	   float tx1=0,tx2=0,ty1=0,ty2=0;
	 //  if(abs(state->ballVel.x)>30)
	  // {
			  for(int i=1;i<10;i++)
			  {
				  xpos[i]=d*i;
				  if(m==INF)
					  ypos[i] = INF ;
				  else
	    			  ypos[i]=state->ballPos.y+ m*(xpos[i]-state->ballPos.x);
				 
				  if(abs(state->ballVel.x)>10)
				    tx1=abs((xpos[i]-state->ballPos.x)/state->ballVel.x);
				  else
			        tx1=INF;
				 
				  if((ypos[i]==INF)||abs(state->ballVel.y)<10)
					  ty1 = INF ;
				  else
					  ty1=abs((ypos[i]-state->ballPos.y)/state->ballVel.y);
				 
				  dest.x = xpos[i] ; dest.y = ypos[i] ;
			//	no use of this condition ::  if(abs(state->homeVel[botID].x)>10)
				  tx2 = (Vector2D<int>::dist(state->homePos[botID],dest))/vbot; 
				  ty2 = (Vector2D<int>::dist(state->homePos[botID],dest))/vbot; 
				  // earlier equation was wrong 

// edited till here ........................................................................................................
// continue further from here...............................................................................................
				  if(abs(ypos[i])>OUR_GOAL_MAXY+3*BOT_RADIUS)
					  time_error[i] = INF ;
				  else
					  if(tx1 == INF || ty1 == INF )
						  time_error[i] = INF ;
					  else  
				          time_error[i]=abs((tx1-tx2)*(ty1-ty2));
		  
			//   outfile<<"i : "<<i<<" xpos[i] : "<<xpos[i]<<" ypos[i] : "<<ypos[i]<<" time_error"<<time_error[i]<<endl ;
			 //  outfile.close();
 			  }
			  time_error[0] = INF;
			  float min=time_error[0];
			  for(int i=1;i<10;i++)
			  {
				  if(time_error[i]<min) {
					  minimum=i;
					  min = time_error[i] ;
				  }
			  }

			  dest.x=xpos[minimum];
			  dest.y=ypos[minimum];
	    
			  sprintf(debug,"%f :: %d :: %d :: %d ",time_error[minimum],minimum,dest.x,dest.y);
		      Client::debugClient->SendMessages(debug);

	  //}
	 /*
	   else
	  {
		  
		  d=HALF_FIELD_MAXY/100;
		  dest.x=state->ballPos.x;
		  for(int i=1;i<100;i++)
		  {
			  ypos[i]=d*i*SGN(state->ballVel.y);
			  if(abs(state->ballVel.y)>10) 
			  ty1=abs((state->ballPos.y-ypos[i])/state->ballVel.y);
			  else
			  ty1=0;
			  ty2=abs((state->homePos[botID].y-ypos[i]/vbot));
			  time_error[i]=abs(ty1-ty2);
		  
		  }
		  float min=time_error[0];
		   for(int i=1;i<100;i++)
			  {
				  if(min>time_error[i]) minimum=i;
			  }
		   dest.y=ypos[minimum];

		   
	    sprintf(debug,"goign in else %f :: %d :: %d :: %d ",time_error[minimum],minimum,dest.x,dest.y);
		Client::debugClient->SendMessages(debug);

	  }
	   */
	  float finalangle=Vector2D<int>::angle(dest,Vector2D<int> (OPP_GOAL_X,0));
	  int finalvel=0;
	  //sprintf(debug,"%f  %f  %f  %d  %d \n",state->ballVel.x,state->ballVel.y,time_error[minimum],dest.x,dest.y);
	//	Client::debugClient->SendMessages(debug);
	    _goToPoint(botID, dest, finalvel,finalangle, CLEARANCE_PATH_PLANNER,1,1);


  }
}
