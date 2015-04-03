//We need to mark the bot in such a way that the bot always comes in between the marked opponent bot and the ball...
//we implement this by using the defend point tactic...


#ifndef TMARKBOT_HPP
#define TMARKBOT_HPP
#define BOT_DIS_THRESH 0.5
#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TMarkBot : public Tactic
  {
  public:
    TMarkBot(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {

    } // TMarkBot

    ~TMarkBot()
    { } // ~TMarkBot

    inline bool isActiveTactic(void) const
    {
      return false;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_bot = (state->homePos[*it] - state->awayPos[*it]).absSq();
        if(dis_from_bot < mindis)
        {
          dis_from_bot = mindis;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      // Select the skill to the executed next
      // if((state->pr_ballOurSide) == true)
      //{
      //if((state->pr_oppBall) == true)
      //{
      //set the params for goToPoint
      //sParam.DefendPointP.x = state->awayPos[tParam.MarkBotP.awayBotID].x;
      //sParam.DefendPointP.y = state->awayPos[tParam.MarkBotP.awayBotID].y;
      //sID = SkillSet::DefendPoint ;
      //}
      //}
      /*else if((state->pr_ballOppSide) == true)
      {

        sID = SkillSet::GoToBall ;
      }
      else if((state->pr_ballMidField) == true)
      {

        sParam.DefendPointP.x=state->awayPos[botID].x;
        sParam.DefendPointP.y=state->awayPos[botID].y;
        sID = SkillSet::DefendPoint ;
      } */
      // Execute the selected skill
      //skillSet->executeSkill(sID, sParam);

      if ((state->homePos[botID] - state->awayPos[botID]).absSq() < BOT_DIS_THRESH * BOT_DIS_THRESH)
      {
        tState = COMPLETED;
      }
    }
  }; // class TMarkBot
} // namespace MyStrategy

#endif // TMARKBOT_HPP
