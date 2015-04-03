#include "stdafx.h"
#include "comdef.h"
#include "naivePS.h"
#include "../../Core/beliefState.h"

using namespace Util;
#define D_PS 1
namespace MyStrategy
{
  NaivePS::NaivePS(const BeliefState* state) 
    : PS(state)
  { } // NaivePS

  NaivePS::~NaivePS()
  { } // ~NaivePS

  void NaivePS::updateWeights(Play::Result termResult) const
  {
    assert(termResult != Play::NOT_TERMINATED); // Play must terminate before updating weights
   // //debug(D_PS, "Terminated with:", termResult);
    float mult;

    switch (termResult)
    {
    case Play::TIMED_OUT:
      mult = 0.95f;
      break;
    case Play::COMPLETED:
      mult = 1.f;break;
      
    case Play::FAILURE_LOW:
      mult = 0.95f; break;
    case Play::FAILURE_MED:
      mult = 0.9f;  break;
    case Play::FAILURE_HIGH:
      mult = 0.8f; break;
      break;

    case Play::SUCCESS_LOW:
      mult = 1.05f; break;
    case Play::SUCCESS_MED:
      mult = 1.1f; break;
    case Play::SUCCESS_HIGH:
      mult = 1.3f; break;

    default:
      ////debug(D_PS, "Termination condition unhandled");
      // Loggerabort("Unhandled play done return value");
		break;
    }
    
    // TODO Do the post-processing here
    playList[playID]->weight *= mult;
  }
} // namespace MyStrategy
