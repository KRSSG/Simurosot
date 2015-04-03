#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#define GOAL_Y 240
#define AWAYGOAL_X 640
#define ANGLE_THRESH 0.1
namespace MyStrategy
{
 
	
	
	
int oscillation()
 {
 static int ticks = 0;
 static int flag = 1;
 
 if((ticks%15)==0)   //set ur own ticks count *******
   {
	 if(flag==1)
		flag = -1;
	 else
		flag = 1;
	}
 ticks++;
 if(ticks>1500)
	 ticks = 0;
return flag ; 
  }

//bool approaching_from_corner();



 void SkillSet::goalKeeping(const SParam &param)
  {
    Vector2D<int> joinPoint, botDestination, point;
   float ang1;
	if((state->ballPos.x)>=0)
	{
	   if(abs(state->ballVel.x)<10)
		   ang1 = 0 ;
	   else
	   	ang1 = atan(state->ballVel.y/state->ballVel.x);
	    
	   joinPoint.y = state->ballPos.y - ((state->ballPos.x) - (-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(ang1) ; //tan(ang1)

	   if(abs(joinPoint.y)>OUR_GOAL_MAXY)
           joinPoint.y = state->ballPos.y;	
	  /*
	   if(abs(joinPoint.y)<OUR_GOAL_MAXY)
	   {
	    Vector2D<int> stablePoint(ForwardX(-HALF_FIELD_MAXX + GOAL_DEPTH),0);
		float ang2 = Vector2D<int>::angle(stablePoint,state->ballPos);
		float finalAngle = (0.9*ang2 + 0.1*ang1)/(0.9 + 0.1);   //weighted final angle to bring stability 
		joinPoint.y = state->ballPos.y - (state->ballPos.x - (-HALF_FIELD_MAXX + DBOX_WIDTH))*tan(finalAngle) ;
	   }
      */
	   if(joinPoint.y >=  OUR_GOAL_MAXY)
		   joinPoint.y = OUR_GOAL_MAXY - 0.2*BOT_RADIUS;
	   if(joinPoint.y <= OUR_GOAL_MINY)
		   joinPoint.y = OUR_GOAL_MINY + 0.2*BOT_RADIUS;

	point.x = (-HALF_FIELD_MAXX + GOAL_DEPTH+0.7*BOT_RADIUS); //+ 100;   //set your threshold ********
	point.y = joinPoint.y ;//+ oscillation()*BOT_RADIUS ;// set according to you decide to put oscillation at normal point********
	
	}
	else
	  {	
        //**********for  corner movement save  *** CHECK FOR THE POSITIONING POSITION**************
	 if(((state->ballPos.x) > (-HALF_FIELD_MAXX + GOAL_DEPTH -5))&&((state->ballPos.x) < (-HALF_FIELD_MAXX + GOAL_DEPTH + 0.7*BOT_RADIUS + BOT_RADIUS))&&(abs(state->ballPos.y) < HALF_FIELD_MAXY/*OUR_GOAL_MAXY + 6*BOT_RADIUS*/)&&(abs(state->ballPos.y) > OUR_GOAL_MAXY))
	   {
		if(state->ballPos.y>OUR_GOAL_MAXY)
		  {
			botDestination.x = (-HALF_FIELD_MAXX + GOAL_DEPTH - 0.5*BOT_RADIUS);//0.7*BOT_RADIUS;
			botDestination.y = OUR_GOAL_MAXY-50;//0.5*BOT_RADIUS ;
            
		}
		if(state->ballPos.y <OUR_GOAL_MINY)
			  	  {
			  botDestination.x = (-HALF_FIELD_MAXX + GOAL_DEPTH -0.5*BOT_RADIUS);// 0.7*BOT_RADIUS;
			  botDestination.y = OUR_GOAL_MINY + 50;//0.5*BOT_RADIUS;
		     
		}
        int opID =  state->oppBotNearestToBall;
		if(float oppdist = Vector2D<int>::dist(state->awayPos[opID],state->ballPos)< 3*BOT_BALL_THRESH)
		{ float dist2  = Vector2D<int>::dist(state->homePos[botID],botDestination);
	    //extraa condition ****************************************
		  if(dist2 > BOT_BALL_THRESH)
			{ _goToPoint(botID,botDestination,0,-PI/2,0);
          //  return;  
			} 
		 return; 
		}
    //***********************************************************


		  }      //*************code left for case where ball is behind bot******************
	   else
	   {
		if(abs(state->ballPos.y)>=(OUR_GOAL_MAXY+ 4*BOT_RADIUS))   
		 { if(state->ballPos.y<=(OUR_GOAL_MINY - 4*BOT_RADIUS))
		  {   botDestination.y = OUR_GOAL_MINY - 0.5*BOT_RADIUS + oscillation()*BOT_RADIUS ; //**********
		      botDestination.x = (-HALF_FIELD_MAXX + GOAL_DEPTH +0.7*BOT_RADIUS );
		  }        	  
	      if(state->ballPos.y>= (OUR_GOAL_MAXY +2*BOT_RADIUS))
		  {   botDestination.y = OUR_GOAL_MAXY + 0.5*BOT_RADIUS + oscillation()*BOT_RADIUS; //***********
		      botDestination.x = (-HALF_FIELD_MAXX + GOAL_DEPTH + 0.7*BOT_RADIUS );
		  }  
		} 
		else
		  {  if(abs(state->ballVel.x)<10)
		       ang1 = 0 ;
	         else
	   	       ang1 = atan(state->ballVel.y/state->ballVel.x);
	         
		    botDestination.y = state->ballPos.y - ((state->ballPos.x) - (-HALF_FIELD_MAXX + GOAL_DEPTH + 0.7*BOT_RADIUS))*tan(ang1) ; //tan(angle)

			
			if(abs(botDestination.y)>OUR_GOAL_MAXY)
				botDestination.y = state->ballPos.y;
		  /* else
			  if(state->ballPos.y > state->homePos[botID].y)
				 if(botDestination.y < state->ballPos.y)
					 botDestination.y = botDestination.y + 20;
				 else
                     botDestination.y = botDestination.y + 20;  // for correct prediction 
		*/	   
			
			if(botDestination.y >=  OUR_GOAL_MAXY)
		          botDestination.y = OUR_GOAL_MAXY -50;// 0.5*BOT_RADIUS;
	            if(botDestination.y <= OUR_GOAL_MINY)
		          botDestination.y = OUR_GOAL_MINY + 50;// 0.5*BOT_RADIUS;
			  
			  botDestination.x = (-HALF_FIELD_MAXX + GOAL_DEPTH + 0.7*BOT_RADIUS);
          
		  
		  
		  }
	   }
		  
	    point.x = botDestination.x ;  //
	    point.y = botDestination.y ;//+ oscillation()*BOT_RADIUS;
	
	 }
 	
 
    
  _goToPoint(botID,point,0,-PI/2,0);  // PI    //set ur angle ******at PI :: to much of disturbation , -PI/2 :: gap is being left ::@oscillation
   // _goToPoint(botID,point,0,PI/2);
  if(float distance = Vector2D<int>::dist(state->ballPos,state->homePos[botID])< 1.2*BOT_BALL_THRESH) //1.2
  { if(FIELD_IS_INVERTED == false)
     { 
	   if(state->ballPos.y>state->homePos[botID].y)
	     comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED);
       else
         comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED); 
     }
  else
  {
     if(state->ballPos.y>state->homePos[botID].y)
	     comm->sendCommand(botID,MAX_BOT_SPEED,-MAX_BOT_SPEED); //
       else
         comm->sendCommand(botID,-MAX_BOT_SPEED,MAX_BOT_SPEED); //
  }
 }
/*
bool approaching_from_corner()
{
	if((state->ballPos.x > ForwardX(-HALF_FIELD_MAXX))&&(state->ballPos.x < ForwardX(-HALF_FIELD_MAXX + BOT_RADIUS)))
	{
	  if(abs(state->ballPos.y) > HALF_FIELD_MAXY)
	  {
	   if(fabs(state->ballVel.x) < 20)
		   return true;
	   else
		   return false ;
	  }
	  else 
		  return false;
	
	}
	else
		return false;
}
*/
}
}  
