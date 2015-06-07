#pragma once
#include "stdafx.h"
#include <iostream>
#include <cstring>
#include <ctime>
#include "robot.h"
#include "../Core/beliefState.h"
#include "../Skills/skillSet.h"
#include "../common/include/cs.hpp"
#include "../winDebugger/Client.h"
#include "tinclude.h"
using namespace std;
using namespace Util;
using namespace HAL;

namespace MyStrategy
{
  
//  Util::CS*  Robot::reader_mutex = new Util::CS();
//  Util::CS*  Robot::writer_mutex =0;
// Util::CS*  Robot::writer_preference = 0;
  int        Robot::reader_count = 0;
  Robot::Robot(BeliefState& stateSh,
               CS*          tacticCS,
               int          botID) :
    tIDSh(Tactic::Stop),
    tStateSh(Tactic::INIT),
    botID(botID),
    tacticCS(tacticCS),
    stateSh(stateSh)
  {
	 
    state = new BeliefState();  // Initializing with default constructor to avoid re-instantaition of referee box and vision thread
    
    // Resetting the tactic list
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      tacticList[tID] = NULL;
    }

    // List all Tactic ID -> Tactic object pairs here for each robot
    tacticList[Tactic::Block]               = new TBlock(state, botID);
   // tacticList[Tactic::ChargeBall]          = new TCharge(state, botID);
    tacticList[Tactic::CoverGoal]           = new TCoverGoal(state,botID);
   // tacticList[Tactic::DragToGoal]          = new TDragToGoal(state, botID);    
    tacticList[Tactic::DefendLine]          = new TDefendLine(state, botID);
    tacticList[Tactic::DefendPoint]         = new TDefendPoint(state, botID);
    tacticList[Tactic::GoalieOur]           = new TGoalKeepingOurSide(state, botID);
    tacticList[Tactic::GoalieOpp]           = new TGoalKeepingOppSide(state, botID);
   // tacticList[Tactic::MarkBot]             = new TMarkBot(state, botID);
	tacticList[Tactic::MidField]             = new TMidField(state, botID);
    //tacticList[Tactic::Pass]              = new TPass(state, botID);
  //  tacticList[Tactic::Backup]              = new TBackup(state, botID);
    tacticList[Tactic::Position]            = new TPosition(state, botID);
    //tacticList[Tactic::PositionForPenalty]  = new TPositionForPenalty(state, botID);
    //tacticList[Tactic::PositionForStart]    = new TPositionForStart(state, botID);
   // tacticList[Tactic::Defend]              = new TDefend(state,botID);
    tacticList[Tactic::Attack]              = new TAttack(state,botID);
    //tacticList[Tactic::Shoot]             = new TShoot(state, botID);
   // tacticList[Tactic::Steal]               = new TSteal(state, botID);
    tacticList[Tactic::Stop]                = new TStop(state, botID);
    tacticList[Tactic::Velocity]            = new TVelocity(state, botID);
    //tacticList[Tactic::PositionForReceive]= new TPositionForReceive(state, botID);
    tacticList[Tactic::ReceiveBall]         = new TReceiveBall(state, botID);
    tacticList[Tactic::AttackLingo]         = new TAttackLingo(state,botID);
	tacticList[Tactic::AttackNew]           = new TAttackNew(state,botID);
	//tacticList[Tactic::ChargeToGoal]		= new TChargeToGoal(state,botID);
   // tacticList[Tactic::Attacksupport]         = new TAttacksupport(state,botID);
	// Initialization check
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      assert(tacticList[tID] != NULL); // Tactic enum not mapped to the corresponding Tactic object
    }
 //   this->start();
  } // Robot

  Robot::~Robot()
  {
    delete state;
    
    for (int tID = 0; tID < Tactic::MAX_TACTICS; ++tID)
    {
      if (tacticList[tID] != NULL)
      {
        delete tacticList[tID];
      }
    }
  } // ~Robot

  void Robot::run()
  {
    Tactic::ID    tID = Tactic::Stop;
    Tactic::Param tParam;
    bool          updated;

   // while (true)
    {
      updated = false;
      // Updating the belief state
      // Reader with Writer preference code (Entry)
	 //   //Client::debugClient->SendMessages("Reacced Here 1\n");
      //reader_mutex->enter();
	   // //Client::debugClient->SendMessages("Reacced Here 2\n");
      if(reader_count == 0)
		  
     //   writer_mutex->enter(); 
		 
      reader_count++;
  //    reader_mutex->leave();
	//  //Client::debugClient->SendMessages("Reacced Here 5\n");
      // Reader (Entry ends)
      if (state->currFrameNum != stateSh.currFrameNum)
      {
        updated = true;
        memcpy(state, &stateSh, sizeof(BeliefState));
      }
      // Reader (Exit)
   //   reader_mutex->enter();
      if(reader_count == 1)
   //     writer_mutex->leave();
      reader_count--;
	 //   //Client::debugClient->SendMessages("Reacced Here 3\n");
     // reader_mutex->leave();
	  // //Client::debugClient->SendMessages("Reacced Here 4\n");
      // Reader (Exit ends)

      // Updating the tactic and its parameters to be executed next
     // tacticCS->enter();
      tID    = tIDSh;
      tParam = tParamSh;
	  	
      if (tStateSh == Tactic::INIT)
      {
        tStateSh = Tactic::RUNNING;
        tacticList[tID]->initialize();
      }
    //  tacticCS->leave();
	
      if (updated)
      {
        tacticList[tID]->execute(tParam);
      }
	  
     // sleep(16);  // Adding sleep to this thread of execution to prevent CPU hogging
    }
  } // run
} // namespace MyStrategy
