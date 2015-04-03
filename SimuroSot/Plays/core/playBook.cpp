#include "../../common/include/config.h"
#include "stdafx.h"

#include <cstdlib>
#include "playBook.h"
#include "play.hpp"

#include "../playbook/pDefense1.hpp"
#include "../playbook/pDefense2.hpp"
#include "../playbook/pNone.hpp"
#include "../playbook/pOffense1.hpp"
#include "../playbook/pOffense2.hpp"
#include "../playbook/pOurGoalKick.hpp"
#include "../playbook/pOppGoalKick.hpp"
#include "../playbook/pPositionOppFreeKick.hpp"
#include "../playbook/pPositionOppStrike.hpp"
#include "../playbook/pPositionOurFreeKick.hpp"
#include "../playbook/pPositionOurStrike.hpp"
#include "../playbook/pSetPosition.hpp"
#include "../playbook/pStop.hpp"
#include "../playbook/pSuperOffense.hpp"
#include "../playbook/pSuperDefense.hpp"
#include "../playbook/pTestPlay.hpp"
#include "../playbook/pOffense3.hpp"
namespace MyStrategy
{
	
  PlayBook::PlayBook(const BeliefState* state) : 
    state(state)
  {
    reload();
  } // PlayBook

  PlayBook::~PlayBook()
  { } // ~PlayBook

  void PlayBook::reload(void)
  {
    // Initializing all play objects with NULL
    for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
    {
      playList[pID] = NULL;
    }

    // Inserting all the name-object pairs into the  Play List
    playList[PlayBook::None]                  = new PNone(*state);
    playList[PlayBook::TestPlay]              = new PTestPlay(*state);
    playList[PlayBook::SetPosition]           = new PSetPosition(*state);
    playList[PlayBook::PositionOurStrike]     = new PPositionOurStrike(*state);
    playList[PlayBook::PositionOppStrike]     = new PPositionOppStrike(*state);
    playList[PlayBook::Stop]                  = new PStop(*state);
    playList[PlayBook::OurGoalKick]           = new POurGoalKick(*state);
    playList[PlayBook::OppGoalKick]           = new POppGoalKick(*state);
    playList[PlayBook::OurFreeKick]           = new PPositionOurFreeKick(*state);
    playList[PlayBook::OppFreeKick]           = new PPositionOppFreeKick(*state);
    playList[PlayBook::SuperOffense]          = new PSuperOffensePlay(*state);
    playList[PlayBook::Offense1]              = new POffense1Play(*state);
    playList[PlayBook::Offense2]              = new POffense2Play(*state);
	playList[PlayBook::Offense3]			  = new POffense3Play(*state);
    playList[PlayBook::SuperDefense]          = new PSuperDefensePlay(*state);
    playList[PlayBook::Defense1]              = new PDefense1Play(*state);
    playList[PlayBook::Defense2]              = new PDefense2Play(*state);
    //
 //	playList[PlayBook::PositionOurKickoff]    = new PPositionOurKickoff(*state);
	//	playList[PlayBook::PositionOppKickoff]    = new PPositionOppKickoff(*state);
		//playList[PlayBook::PositionOurPenalty]    = new PPositionOurPenalty(*state);
	//	playList[PlayBook::PositionOppPenalty]    = new PPositionOppPenalty(*state);
		//playList[PlayBook::PositionOurFreeBall]    = new PPositionOurFreeBall(*state);
	//	playList[PlayBook::PositionOppFreeBall]    = new PPositionOppFreeBall(*state);
		playList[PlayBook::PositionOurFreeKick]    = new PPositionOurFreeKick(*state);
		playList[PlayBook::PositionOppFreeKick]    = new PPositionOppFreeKick(*state);
	//	playList[PlayBook::PositionOurGoalKick]    = new PPositionOurGoalKick(*state);
		//playList[PlayBook::PositionOppGoalKick]    = new PPositionOppGoalKick(*state);
	//
    
    // Checking if all the play ids have been associated with their corresponding Play objects
    for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
    {
      assert(playList[pID] != NULL); // Play has not been assigned a Play object
    }
  }
}
