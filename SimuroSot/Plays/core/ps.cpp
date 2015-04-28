/* This file contains the PS class(Play Selector) that implements the algorithm
 * to select the play to be implemented in each frame. It uses the probabilistic model 
 * to select the most appropriate play among the list of applicable plays.
 */
#include "stdafx.h"
#include <cstdlib>
#include "ps.h"
#include "playBook.h"
#include "play.hpp"
#include "../../winDebugger/Client.h"

using namespace std;
#define D_PS 1
namespace MyStrategy
{
  PS::PS(const BeliefState* state) : 
    PlayBook(state),
    playID(PlayBook::None)
  {
    for (int playID=0; playID<PlayBook::MAX_PLAYS; ++playID)
    {
      appl[playID] = false;
      pProb[playID] = 0.0f;
    }
 }

  PS::~PS()
  { }

  void PS::select(void)
  {
    // Find the applicable plays
    const int num_plays = PlayBook::None;
	char deb[200];
	sprintf(deb,"Selecting Next Play...\n");
//	Client::debugClient->SendMessages(deb);
	//sprintf(deb,"Ball Our: %d Opp: %d Loose: %d\n", state->pr_ourBall,state->pr_oppBall,state->pr_looseBall);
	//Client::debugClient->SendMessages(deb);
    //debug(D_PS, "Selecting Next Play...");
    //debug(D_PS, "Game Running: ", state->pr_gameRunning);
    //debug(D_PS, "Ball Our:", state->pr_ourBall, " Opp:", state->pr_oppBall, " Loose:", state->pr_looseBall);
    for (int pID = 0; pID < num_plays; ++pID)
    {
      appl[pID] = playList[pID]->applicable();
      //debug(D_PS, "Play:", playList[pID]->name, 
//            "Applicability:", appl[pID], "Weight:", playList[pID]->weight);
	  sprintf(deb,"Play ID: %d Applicability: %d Weight: %f\n", pID,appl[pID],playList[pID]->weight);
	//  Client::debugClient->SendMessages(deb);
    }

    float cumWeight = 0.0f;
    for (int pID = 0; pID < num_plays; ++pID)
    {
      if (appl[pID])
      {
        cumWeight += playList[pID]->weight;
      }
    }

    for (int pID = 0; pID < num_plays; ++pID)
    {
      if (appl[pID])
      {
        pProb[pID] = playList[pID]->weight/cumWeight;
      }
      else
      {
        pProb[pID] = 0.0f;
      }
    }

    float cumProb = 0.0f;
    float randVal = (float)std::rand()/RAND_MAX + 0.000001f;
    if (randVal >= 1.0f)
    {
      randVal = 0.999999f;
    }

	playID = PlayBook::PositionOppFreeKick;
 /*    
	for (int pID = 0; pID < num_plays; ++pID)
    {
      cumProb += pProb[pID];
      if (cumProb >= randVal && appl[pID])
      {
        playID = (PlayID)pID;
        break;
      }
    }
 */
    sprintf(deb,"selected play %d\n",playID);
   //  Client::debugClient->SendMessages(deb);
    assert(playID != PlayBook::None); // No play selected

	//playID = PlayBook::Offense2;
    playList[playID]->startTimer();
  } // select
} // namespace MyStrategy
