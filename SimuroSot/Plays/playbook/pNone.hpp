#ifndef PNONE_HPP
#define PNONE_HPP

#include <utility>
#include "../core/play.hpp"
#include "../../Core/beliefState.h"
#include "../../Tactics/tactic.h"
#include "../../common/include/config.h"

namespace MyStrategy
{
  class PNone : public Play
  {
  public:
    inline PNone(const BeliefState& state) 
      : Play(state)
    {
      name = "None";
    }

    inline ~PNone()
    { }

    inline bool applicable(void) const
    {
      return false; // Should always be false as this the None play
    }

    inline Result done(void) const
    {
      return COMPLETED;
    }
  }; // class PNone
} // namespace Strategy

#endif // PNONE_HPP
