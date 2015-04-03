#include "stdafx.h"
#include "trainers.hpp"
#include "../Tactics/tinclude.h"
#include <iostream>
using namespace MyStrategy;
using namespace std;
#define D_TESTT 1
void start_TacticTester(Kalman &kFilter, BeliefState &state, bool &run)
{
  TGoalKeepingOurSide tGoalOur(&state, 0);
  Tactic::Param paramGoal;
  TGoalKeepingOppSide tGoalOpp(&state, 1);

  TStop tS0(&state, 0);
  TStop tS1(&state, 1);
  TStop tS2(&state, 2);
  Tactic::Param paramStop;
  

  TPosition tPos(&state, 1);
  Tactic::Param pPos;
  pPos.PositionP.align = true;
  pPos.PositionP.x= 0;
  pPos.PositionP.y= 0;
  pPos.PositionP.finalSlope=0;
  
  TVelocity tVelocity(&state,0);
  Tactic::Param pVelocity;
  pVelocity.VelocityP.vl = -20;
  pVelocity.VelocityP.vr = 20;
//    
//    TSteal tSteal(&state, 2);
//    Tactic::Param pSteal;
//    
  TAttack tAttack1(&state, 1);
  TAttack tAttack0(&state, 0);
  TAttack tAttack2(&state, 2);
  Tactic::Param pAttack;
  /*FILE *f = fopen("/home/robo/botplot/compare_dataset/botlog.txt", "w");
  fclose(f);
  f = fopen("/home/robo/botplot/compare_dataset/response.txt", "w");
  fclose(f);*/
  bool isRunning = true; 
  //    Util::Timer timer;
  while(run)
  {
  //      timer.start();
    kFilter.update(state);
    if(1)
    {
  //        printf("Bot Pos = %d, %d\n", state.homePos[0].x, state.homePos[0].y);
      printf("Ball Pos = %d, %d\n", state.ballPos.x, state.ballPos.y);
      //tGoalOur.execute(paramGoal);
  //  tGoalOpp.execute(paramGoal);
   //  goalkeep.execute(pd);
       // Sleep(10000);
      //tvel.execute(tpV);
  //        tpD.execute(paramD);
  //        tpD.execute(pd);
  //        tAttack0.execute(pAttack);
        tAttack0.execute(pAttack);
  //        tVelocity.execute(pVelocity);
       
     // Sleep(10000);
  //        defendline.execute(paramDefend);
     // tPos.execute(pPos);
  //       t.execute(tpV);
  //     turnangle.executeSkill(SkillSet::TurnToAngle, sparam);
    }
    else
    {
      printf("OFF!\n");
      tS0.execute(paramStop);
      tS1.execute(paramStop);
      tS2.execute(paramStop);
    }
  //      printf("tIMER US = %d\n", timer.stopus() );
    //moprintf("Ball Pos: %d %d %f\n",state.ballPos.x,state.ballPos.y,state.homeAngle[2]);
  //      Sleep(10000);  // Adding sleep to this thread of execution to prevent CPU hogging
    
  }
 /* int cnt = 1000;
  int st = 0; // caching and passing
  while(run)
  {
    //debug(D_TESTT, st, cnt);
    kFilter.update(state);
    switch(st) {
      case 0:
        tp_pass.execute(tparam_pass);
//        tp_posforreceive.execute(tparam_posforreceive);
        if(tp_pass.tState == Tactic::COMPLETED) {
          st = 1;
          tp_pass.tState = Tactic::RUNNING;
        }
        break;
      case 1:
        tp_receiveball.execute(tparam_receiveball);
        cnt--;
        if(cnt <= 0) {
          cnt = 1000;
          st = 0;
        }
        break;
    }
    Sleep(10000);  // Adding sleep to this thread of execution to prevent CPU hogging
  }*/
}