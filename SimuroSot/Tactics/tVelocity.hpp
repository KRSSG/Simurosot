#ifndef TVELOCITY_HPP
#define TVELOCITY_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TVelocity : public Tactic
  {
  public:
    TVelocity(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TVelocity

    ~TVelocity()
    { } // ~TVelocity

    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        return *it;
      }
     // Util::// Loggerabort("No bot available for role assignment");
      return -1;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      // Select the skill to the executed next
      sID = SkillSet::Velocity;
      sParam.VelocityP.vl = tParam.VelocityP.vl;
      sParam.VelocityP.vr = tParam.VelocityP.vr;      
      // Execute the selected skill
      skillSet->executeSkill(sID, sParam);

      if (state->homeVel[botID].absSq() > ZERO_VELOCITY_THRESHOLD * ZERO_VELOCITY_THRESHOLD)
      {
        tState = COMPLETED;
      }
    }
  }; // class TVelocity
} // namespace MyStrategy

#endif // TVELOCITY_HPP
