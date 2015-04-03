#include "stdafx.h"
#include <list>
#include "pExec.h"
#include "../../Tactics/robot.h"
#include "../../common/include/cs.hpp"
#include "../../winDebugger/Client.h"
using namespace std;
using namespace Util;
using namespace HAL;
#define D_PEXEC 1

namespace MyStrategy
{
  PExec::PExec(BeliefState* state) :
    PS_CLASS(state)
  {
	 
    for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
      tacticCS[botID] = new CS();
     robot[botID]    = new Robot(*state, tacticCS[botID], botID);
    }
  } // PExec

  PExec::~PExec()
  {
    for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
      // TODO Deleting the tacticCS causes SIGABORT. need to correct this.
      // delete tacticCS[botID];
      // delete robot[botID];
    }
  } // ~PExec

  void PExec::assignRoles(bool isReassign)
  {

   // //debug(D_PEXEC, "Reassigning Roles", isReassign);
    if (playID == PlayBook::None)
    {
      return;
    }
    Play* currPlay = playList[playID];
    bool goodBot[5] = {true, true, true, true, true};
    int goodBotCount = 5;
    // Initialization
    list<int> freeBots;
    for (int botID = 0; botID < HomeTeam::SIZE; ++botID) // Iterating over all bots - making them all free for role allocation
    {
      if(goodBot[botID])
      freeBots.push_back(botID);
    }
    for (int roleIdx = 0; roleIdx < HomeTeam::SIZE; ++roleIdx) // Iterating over all roles
    {
      if (currTacticIdx < currPlay->roleList[roleIdx].size()) // Tactic exists for the current role iteration
      {
        Tactic::ID    tID     = currPlay->roleList[roleIdx][currTacticIdx].first;
        Tactic::Param tParam  = currPlay->roleList[roleIdx][currTacticIdx].second;
        
        int           bestBot;

        /* Assign the Goalie role to always the bot id 0 */
        //if(roleIdx != 0)
        bestBot = robot[roleIdx]->tacticList[tID]->chooseBestBot(freeBots, &tParam);

        freeBots.remove(bestBot);
        currPlay->assignedBot[roleIdx] = bestBot;
        // Saving the current tactic to be executed by the bot selected
        currTactic[bestBot]   = currPlay->roleList[roleIdx][currTacticIdx];
       // Util::// LoggertoStdOut("Updating tactics\n");
        // Updating the tactic of the selected bot
        tacticCS[bestBot]->enter();
        robot[bestBot]->tIDSh    = tID;
        robot[bestBot]->tacticList[tID]->tState = (isReassign?(Tactic::RUNNING):(Tactic::INIT));
        robot[bestBot]->tParamSh = tParam;
        robot[bestBot]->tStateSh = (isReassign?(Tactic::RUNNING):(Tactic::INIT));
        tacticCS[bestBot]->leave();
      }
      if(roleIdx == goodBotCount-1) {
        for (int botID = 0; botID < HomeTeam::SIZE; ++botID) // Iterating over all bots - making them all free for role allocation
        {
          if(!goodBot[botID])
          freeBots.push_back(botID);
        }
      }
    }
    // LoggertoStdOut("Assigned roles for tactic index %d\n", currTacticIdx);
  } // asisgnRoles

  bool PExec::canTransit(void)
  {
    if (playID == PlayBook::None)
    {
      return true;
    }

    if (currTacticIdx >= playList[playID]->maxTacticsPerRole)
    {
      return false;
    }

    Play* currPlay = playList[playID];

    int numActiveTactics = 0;

    for (int roleID = 0; roleID < HomeTeam::SIZE; ++roleID)
    {
      Tactic::ID tID       = currPlay->roleList[roleID][currTacticIdx].first;
      Tactic*    selTactic = robot[currPlay->assignedBot[roleID]]->tacticList[tID];

      if (selTactic->isActiveTactic())
      {
        ++numActiveTactics;

        if (selTactic->tState != Tactic::COMPLETED)
        {
          // If there is at least one incomplete active tactic, then cannot transit
          //Util::// LoggertoStdOut("Active tactic not completed  : %d %d\n",roleID,selTactic->tState);
          return false;
        }
		else{
         // Util::// LoggertoStdOut("Active tactic COMPLETED : %d %d\n",roleID,selTactic->tState);
		}
      }
    }

    if (numActiveTactics > 0)
    {
     // Util::// LoggertoStdOut("ACTIVE TACTIC COMPLETED. TRY Transit");
      return true;  // There is atleast 1 active tactic and all of them have completed hence can transit
    }
    else
    {
      // There are no active tactics in this iteration and hence all the tactics must be completed in order to transit
      for (int roleID = 0; roleID < HomeTeam::SIZE; ++roleID)
      {
        Tactic::ID tID       = currPlay->roleList[roleID][currTacticIdx].first;
        Tactic*    selTactic = robot[roleID]->tacticList[tID];

        if (selTactic->tState != Tactic::COMPLETED)
        {
          // If there is at least one incomplete tactic, then cannot transit
          return false;
        }
      }
    }
    //Util::// LoggertoStdOut("Can Transit returning true.");
    return true;
  } // canTransit

  bool PExec::tryTransit(void)
  {
    if (currTacticIdx + 1 < playList[playID]->maxTacticsPerRole)
    {
      return true;
    }
    //// LoggertoStdOut("Try Transit returning false\n");
    return false;
  } // tryTransit

  void PExec::selectPlay(void)
  {
	
    select();

    playResult = Play::NOT_TERMINATED;
    currTacticIdx = 0;
    assignRoles(false);
  } // selectPlay

  void PExec::executePlay(void)
  {
   // Client::debugClient->SendMessages("????????????????????????????????????\n");
	if (canTransit() && tryTransit())
    {
      ++currTacticIdx;
      assignRoles(false);
    } else if( playList[playID]->shouldRolesReassign ) {
      playList[playID]->shouldRolesReassign = false;
      assignRoles(true);
    }
	 for (int botID = 0; botID < HomeTeam::SIZE; ++botID)
    {
     
		robot[botID]->run();
	 }
  } // executePlay

  void PExec::evaluatePlay(void)
  {
    if (playID == PlayBook::None)
    {
      return;
    }
    
    updateWeights(playResult);
  } // evaluatePlay

  bool PExec::playTerminated(void)
  {
    if (playID == PlayBook::None)
    {
     // Util::// LoggertoStdOut("Last Play was None!\n");
      return true;
    }
    if(playList[playID]->timedOut())
    {
     // Util::// LoggertoStdOut("Play Timed out.\n");
      playResult = Play::TIMED_OUT;
      return true;
    }
    /* The completion of a play is defined to be the completion of all the tactics assigned
     * to all the bots. Since this information is available in the Tactic class and not known
     * to the Play class, the evaluation of the completion of the play is done here
     * instead of being done in the done() function of the individual plays
     */
    playResult = playList[playID]->done();
    if(playResult == Play::NOT_TERMINATED  && playCompleted())
      playResult = Play::COMPLETED;
    return (playResult != Play::NOT_TERMINATED);
  } // playTerminated

  bool PExec::playCompleted(void)
  {
    return canTransit() && !tryTransit();
  }
} // namespace MyStrategy