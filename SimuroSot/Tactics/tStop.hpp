#ifndef TTSTOP_HPP
#define TTSTOP_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TStop : public Tactic
  {
  public:
    TStop(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TTStop

    ~TStop()
    { } // ~TTStop

    inline bool isActiveTactic(void) const
    {
      return false;
    }


    void execute(const Param& tParam)
    {
      // Select the skill to the executed next
      stopBot();

      if (state->homeVel[botID].absSq() < ZERO_VELOCITY_THRESHOLD*ZERO_VELOCITY_THRESHOLD)
      {
        tState = COMPLETED;
      }
    }
  }; // class TTStop
} // namespace MyStrategy

#endif // TTSTOP_HPP
