/* This file updates the weights of the different plays depending upon their outcome:
 * SUCCESS,COMPLETED,FAILURE OR ABORTED.
 */
#include "stdafx.h"
#include <cmath>
#include "comdef.h"
#include "expPS.h"
#include "../../Core/beliefState.h"

using namespace Util;
#define D_EXPS 1
namespace MyStrategy
{
  ExpPS::ExpPS(const BeliefState* state) : 
    PS(state)
  { }

  ExpPS::~ExpPS()
  { }

  void ExpPS::updateWeights(Play::Result termResult) const
  {
    assert(termResult != Play::NOT_TERMINATED); // Play must terminate before updating weights

    static bool init = true;
    float       wcap[PlayBook::MAX_PLAYS];
    float       norm[PlayBook::MAX_PLAYS];
    float       mult[PlayBook::MAX_PLAYS];

    /* Initializing all the multiplication factors to default value of 1.0 */
    for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
    {
      mult[pID] = 1.0f;
    }

    /* Updating multplication factor of the just terminated play */
    switch (termResult)
    {
    case Play::TIMED_OUT:
      mult[playID] = 10.0f/11.0f;
      break;

    case Play::COMPLETED:
      mult[playID] = 11.0f/10.0f;
      break;

    case Play::FAILURE_LOW:
    case Play::FAILURE_MED:
    case Play::FAILURE_HIGH:
      mult[playID] = 2.0f/3.0f;
      break;

    case Play::SUCCESS_LOW:
    case Play::SUCCESS_MED:
    case Play::SUCCESS_HIGH:
      mult[playID] = 3.0f/2.0f;
      break;

    default:
      //debug(D_EXPS, "Unhandled play termination return value");
      break;
    }

    if (init == false)
    {
      // Computing wcap values
      for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
      {
        wcap[pID] = playList[pID]->weight*pow((float)mult[pID], (float)1.0/pProb[pID]);
      }

      // Computing normalization factors
      float num = 0, den = 0;
      for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
      {
        if (appl[pID])
        {
          num += playList[pID]->weight;
          den += wcap[pID];
        }
      }
      for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
      {
        if (appl[pID] == false)
        {
          norm[pID] = 1.0f;
        }
        else
        {
          norm[pID] = num/den;
        }
      }

      // Normalizing the play weights
      for (int pID=0; pID<PlayBook::MAX_PLAYS; ++pID)
      {
        playList[pID]->weight = wcap[pID]*norm[pID];
      }
    }

    init = false;
  }
} // namespace MyStrategy
