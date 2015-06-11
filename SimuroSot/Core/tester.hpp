# pragma once
#include "../mainheaders.hpp"
#include "../common/include/thread.h"
#include "../common/include/cs.hpp"
#include "../Tactics/tactic.h"
#include "../winDebugger/Client.h"

#include "comdef.h"
#include "../Utils/pathPlanners.h"


#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/geometry.hpp"



using namespace std;
using namespace MyStrategy;

class Tester : public Util::Thread
{
public:
  bool &running;
  bool isFirstRun;
  BeliefState &state;
 
  Tester(bool &running_, BeliefState &state_):
	  running(running_),state(state_), isFirstRun(true) {}
  
  void run()
  {
	 
    Util::CS loggerCS;
   //// Util::Logger logger("timelog.log",// Util::// LoggerWrite, loggerCS);
   
	 //TGoalKeepingOppSide tGoalOur(&state, 1);
  //  Tactic::Param paramGoal;
  //  TGoalKeepingOppSide tGoalOpp(&state, 1);

  //  TStop tS0(&state, 0);
  //  TStop tS1(&state, 1);
  //  TStop tS2(&state, 2);
  //  Tactic::Param paramStop;
    
//	static TattackSupport tattacksupport1(&state,1);

	static TBlock tblock1(&state,1);
	static Tactic::Param pBlock;
	pBlock.BlockP.dist = 750;
   static TPosition tPos(&state, 0);
   static Tactic::Param pPos;
    pPos.PositionP.align = true;
    pPos.PositionP.x= -1000;
    pPos.PositionP.y= 2000;
    pPos.PositionP.finalSlope=0;
    
    static TVelocity tVelocity(&state,0);
    static Tactic::Param pVelocity;
    pVelocity.VelocityP.vl = 20;
    pVelocity.VelocityP.vr = 20;

     static MyStrategy::SParam sp;
	 static MyStrategy::SParam sp1 , sp2;
     
	 sp.GoToPointP.x =  0;//+ BOT_RADIUS);
	 sp.GoToPointP.y = 0 ;//+ BOT_RADIUS;
     sp.GoToPointP.align = false;
     sp.GoToPointP.finalslope = PI/2;

	 sp1.GoToPointDWP.x = 0 ;
	 sp1.GoToPointDWP.y = 0 ;
	 sp1.GoToPointDWP.finalslope = PI/2 ;
     
  /*
	 sp.TurnToPointP.x = 0;
	 sp.TurnToPointP.y = 0;
    */

	// static SkillSet::goToBallStraight(&param1);
    static SkillSet p1(&state,0);
   
    static TGoalKeepingOurSide tGoalOur0(&state,0);
	Tactic::Param pgoalie;
	
	static TGoalKeepingOppSide tGoalOpp0(&state,0);
	Tactic::Param pgoaleiopp;
  static TCoverGoal tCover1(&state,1);
	Tactic::Param pcover;
	
  static TAttack tattack1(&state,1);
  static TAttack tattack2(&state,2);
  static TAttack tattack3(&state,3);
  static TAttack tattack4(&state,4);
  Tactic::Param pattack;

  
   static TAttackLingo tattacklingo1(&state,1);
  static TAttackLingo tattacklingo4(&state,4);
  static TAttackLingo tattacklingo3(&state,3);
  static TAttackLingo tattacklingo2(&state,2);
  Tactic::Param pattacklingo;

  static TMidField tmidfield2(&state,2);
  static TMidField tmidfield3(&state,3);
  static TMidField tmidfield4(&state,4);
  Tactic::Param pmid;

  static TBlock tblock3(&state,3);
	Tactic::Param pblock;
	pblock.BlockP.dist = 1750;

  static TReceiveBall tReceive4(&state,4);
  static TReceiveBall tReceive3(&state,3);
  static TReceiveBall tReceive2(&state,2);
  static Tactic::Param pReceive;

  static TDefendLine tDefendLine1(&state,1);
  static Tactic::Param pDefendLine;
  pDefendLine.DefendLineP.x1 = 0;
  pDefendLine.DefendLineP.x2 = -HALF_FIELD_MAXX;
  pDefendLine.DefendLineP.y1 = HALF_FIELD_MAXY;
  pDefendLine.DefendLineP.y2 = 0;

  ////    
  ////    TSteal tSteal(&state, 2);
  ////    Tactic::Param pSteal;
  ////    
  //  TAttack tAttack1(&state, 1);
 static TAttack tAttack0(&state, 0);
 // static  TAttack tAttack2(&state, 2);

 static Tactic::Param pAttack;
    pAttack.AttackP.rotateOnError = true;
    /*FILE *f = fopen("/home/robo/botplot/compare_dataset/botlog.txt", "w");
    fclose(f);
    f = fopen("/home/robo/botplot/compare_dataset/response.txt", "w");
    fclose(f);*/
   isFirstRun= false;
    //    Util::Timer timer;
	  

    {
    //      timer.start();
     char debug[250];
      if(1)
      {
        sprintf(debug,"Ball Pos = %d, %d \n", state.pr_ballOurSide,state.pr_ballOppSide);
		//  sprintf(debug,"Bot Pos = %d, %d    Distance %f\n", state.homePos[1].x, state.homePos[1].y,Vector2D<int>::dist(state.homePos[1],state.ballPos));
		 // Client::debugClient->SendMessages(debug);
		 
		  //********************************************************
		  
		 tGoalOur0.execute(pgoalie);
	//	 tCover1.execute(pcover);

    int dis[3];
	dis[0] = Vector2D<int>::dist(state.homePos[2],state.ballPos) ;
	dis[1] = Vector2D<int>::dist(state.homePos[3],state.ballPos) ;
	dis[2] = Vector2D<int>::dist(state.homePos[4],state.ballPos) ;

	// **** changes made by GUNJAN
	/*
	tCover1.execute(pcover);
	tReceive3.execute(pReceive);
	if(dis[0]<dis[2])
	{
		tattack2.execute(pattack);
        if((abs(state.homePos[2].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[2].x>state.homePos[4].x)||((state.ballPos.x)>HALF_FIELD_MAXX- 2*BOT_RADIUS && (state.homePos[2].x)>HALF_FIELD_MAXX- 2*BOT_RADIUS &&((state.ballPos.y>OPP_GOAL_MAXY && state.ballVel.y>0 && state.homePos[2].y>state.homePos[4].y)||(state.ballPos.y<OPP_GOAL_MINY && state.ballVel.y<0 && state.homePos[2].y<state.homePos[4].y))))
		{		 
		  tattack4.execute(pattacklingo);
		  
		}
		else
		{
		if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
			  tmidfield4.execute(pmid);
		  else
		  tattacklingo4.execute(pattacklingo);
		if(state.homePos[1].x>HALF_FIELD_MAXX*0.5 && abs(state.homePos[1].y) >HALF_FIELD_MAXY-2*BOT_RADIUS) tattack1.execute(pattack);
		else	tCover1.execute(pcover);

		}
	}
	 else if(dis[0]>dis[2])
	{
		tattack4.execute(pattack);
        if((abs(state.homePos[4].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[2].x>state.homePos[2].x)||((state.ballPos.x)>HALF_FIELD_MAXX- 2*BOT_RADIUS && (state.homePos[2].x)>HALF_FIELD_MAXX- 2*BOT_RADIUS &&((state.ballPos.y>OPP_GOAL_MAXY && state.ballVel.y>0 && state.homePos[4].y>state.homePos[2].y)||(state.ballPos.y<OPP_GOAL_MINY && state.ballVel.y<0 && state.homePos[4].y<state.homePos[2].y))))
		{		 
		  tattack2.execute(pattacklingo);
		}
		else
		{
			if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
				  tmidfield2.execute(pmid);
			  else
			  tattacklingo2.execute(pattacklingo);
			  if(state.homePos[1].x>HALF_FIELD_MAXX*0.5 && abs(state.homePos[1].y) >HALF_FIELD_MAXY-2*BOT_RADIUS ) tattack1.execute(pattack);
		      else	tCover1.execute(pcover);
		}
	}	
	
	// ***************************
	//if((dis[0]<dis[1])&&(dis[0]<dis[2]))   //bot 2 is closet
	//{
	//   tattack2.execute(pattack);        //needs to be changed
	//  if(dis[1]<dis[2])
	//  {
	//	  if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield3.execute(pmid);          //needs to be changed for defense rule
	//	  else
	//	      tattacklingo3.execute(pattacklingo);  //lingo    //needs to be changed for attacking rule
	//    
	//	  tReceive4.execute(pReceive);
	//  }
	//  else
	//  { if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield4.execute(pmid);
	//	  else
	//	  tattacklingo4.execute(pattacklingo);  //lingo
	//   tReceive3.execute(pReceive);
	//  }
	//
	//}
 // else
 // {
	//if((dis[1]<dis[0])&&(dis[1]<dis[2]))  //bot 3 nearest
	//{
	//   tattack3.execute(pattack);
	//  if(dis[0]<dis[2])
	//  { if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield2.execute(pmid);
	//	  else
	//	      tattacklingo2.execute(pattacklingo);  //lingo
	//    tReceive4.execute(pReceive);
	//  }
	//  else
	//  { if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield4.execute(pmid);
	//	  else
	//	  tattacklingo4.execute(pattacklingo);  //lingo
	//    tReceive2.execute(pReceive);
	//  }
	//
	//}
 // else
 //  {
	//if((dis[2]<dis[0])&&(dis[2]<dis[1]))   //bot 4 nearest
	//{
	//  tattack4.execute(pattack);
	//  if(dis[0]<dis[1])
	//  { if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield2.execute(pmid);
	//	  else
	//	  tattacklingo2.execute(pattacklingo);   //lingo
	//    tReceive3.execute(pReceive);
	//  }
	//  else
	//  {  if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
	//		  tmidfield3.execute(pmid);
	//	  else
	//	  tattacklingo3.execute(pattacklingo);   //lingo
	//    tReceive2.execute(pReceive);
	//  }
	//
	// }
 //   }
 //  }
 */
	//******************************************* testing - gunjan ************************************

	//tattack2.execute(pattack);
	//tattack4.execute(pattack);
	//tmidfield4.execute(pmid);
	//tattacklingo4.execute(pattacklingo);
	//tCover1.execute(pcover);
	tReceive3.execute(pReceive);
	if(dis[0]<dis[2])
	{
		tattack2.execute(pattack);
        if((abs(state.homePos[2].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[2].x>state.homePos[4].x)||((state.ballPos.x)>HALF_FIELD_MAXX- 2*BOT_RADIUS && (state.homePos[2].x)>HALF_FIELD_MAXX- 2*BOT_RADIUS &&((state.ballPos.y>OPP_GOAL_MAXY && state.ballVel.y>0 && state.homePos[2].y>state.homePos[4].y)||(state.ballPos.y<OPP_GOAL_MINY && state.ballVel.y<0 && state.homePos[2].y<state.homePos[4].y))))
		{		 

		  tattack4.execute(pattacklingo);
		
		  
		}
	   /*	else if((abs(state.homePos[2].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[4].x>state.homePos[2].x))
		{
			tattacksupport4.execute(pattacksupport);
		}*/
		else
		{
		if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
			  tmidfield4.execute(pmid);
		  else
		  tattacklingo4.execute(pattacklingo);
		if((abs(state.ballPos.y)>HALF_FIELD_MAXY-2.5*BOT_RADIUS)|| (state.ballPos.x>HALF_FIELD_MAXX-GOAL_DEPTH-2*BOT_RADIUS)) tattack1.execute(pattack);
		else	tCover1.execute(pcover);

		}
		
	}
	 else if(dis[0]>dis[2])
	{
		tattack4.execute(pattack);
        if((abs(state.homePos[4].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[2].x>state.homePos[2].x)||((state.ballPos.x)>HALF_FIELD_MAXX- 2*BOT_RADIUS && (state.homePos[2].x)>HALF_FIELD_MAXX- 2*BOT_RADIUS &&((state.ballPos.y>OPP_GOAL_MAXY && state.ballVel.y>0 && state.homePos[4].y>state.homePos[2].y)||(state.ballPos.y<OPP_GOAL_MINY && state.ballVel.y<0 && state.homePos[4].y<state.homePos[2].y))))
		{		 
		  tattack2.execute(pattacklingo);
	
		}
		/*else if((abs(state.homePos[4].y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && abs(state.ballPos.y)>HALF_FIELD_MAXY-1.5*BOT_RADIUS && state.homePos[2].x>state.homePos[4].x))
		{
			tattacksupport2.execute(pattacksupport);
		}*/
		else
		{
			if(state.ballPos.x < -0.5*HALF_FIELD_MAXX)
				  tmidfield2.execute(pmid);
			  else
			  tattacklingo2.execute(pattacklingo);
			if((abs(state.ballPos.y)>HALF_FIELD_MAXY-2.5*BOT_RADIUS) || (state.ballPos.x>HALF_FIELD_MAXX-GOAL_DEPTH-2*BOT_RADIUS)) tattack1.execute(pattack);
			else	tCover1.execute(pcover);
			  
		}
	
	}	

	//***********************************************************************
	

       // tGoalOur0.execute(pgoalie);
    //  tGoalOpp.execute(paramGoal);
     //  goalkeep.execute(pd);
     //     Sleep(10000);
        //tvel.execute(tpV);
    //        tpD.execute(paramD);
    //        tpD.execute(pd);
          // tAttack0.execute(pAttack);
		/*
		 if(state.ballPos.y > 0)
		  {
		   tattack4.execute(pattack);
		   tReceive3.execute(pReceive);
		  }
		  else
		  {
		   tattack3.execute(pattack);
		   tReceive4.execute(pReceive);
		  }
		 
	     tmidfield2.execute(pmid);
        */
	//  tGoalOur0.execute(pgoalie);
		//  tGoalOpp0.execute(pgoaleiopp);
	//**********************************************	
	     

	//******************************************

		// tattack2.execute(pattack);
		// tmidfield2.execute(pmid);
		/* if(state.ballPos.y>0)
		 tmidfield2.execute(pmid);
	//	 float dist3=Vector2D<int>::dist(state->ballPos, state->homePos[3]);

		 if(state.ballPos.y>0)
		 {
			 tattack3.execute(pattack);
			 tReceive4.execute(pReceive);
		 }
		 else
		 {
			 tattack4.execute(pattack);
			 tReceive3.execute(pReceive);
		 }
        */
		//		 tattack2.execute(pattack);
		  //tblock3.execute(pblock);
   //  tReceive4.execute(pReceive);
	//  tReceive3.execute(pReceive);
      //tDefendLine1.execute(pDefendLine);
  		//  tbackup4.execute(pbackup);
    // p1.executeSkill(SkillSet::GoToPoint,sp); 
//		  p1.executeSkill(SkillSet::GoalKeeping,sp);
		 // tbackup4.execute(pbackup);
//      p1.executeSkill(SkillSet::GoToPoint,sp); 
       // p1.executeSkill(SkillSet::GoToPointDW,sp1);
        //  p1.executeSkill(SkillSet::TestSkill,sp2) ;      
// tAttack0.execute(pAttack);

		//  if(state.ballPos.x<=19)
          //  tVelocity.execute(pVelocity);
         
  
    //        defendline.execute(paramDefend);
	//	  if(state.ballPos.x>200)
    //  tPos.execute(pPos);
		 //  tGoalOur0.execute(paramGoal);
		//  tCover1.execute(pcover);
		   //       t.execute(tpV);
    //       t.execute(tpV);
//     turnangle.executeSkill(SkillSet::TurnToAngle, sparam);
 //     p1.executeSkill(SkillSet::GoToPointStraight,sp);
      }
      else
      {
        printf("OFF!\n");
       /* tS0.execute(paramStop);
        tS1.execute(paramStop);
        tS2.execute(paramStop);*/
      }
    //      printf("tIMER US = %d\n", timer.stopus() );
      //moprintf("Ball Pos: %d %d %f\n",state.ballPos.x,state.ballPos.y,state.homeAngle[2]);
    //      Sleep(10000);  // Adding sleep to this thread of execution to prevent CPU hogging
     // Sleep(3) ;
    }
   // Util::// LoggertoStdOut("Exiting process");
    return;

  }
};