#ifndef TKICKTOGOAL_HPP
#define TKICKTOGOAL_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TKickToGoal : public Tactic
  {
  public:
    TKickToGoal(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TTStop

    ~TKickToGoal()
    { } // ~TTStop
    enum InternalState
    {
      APPROACHING,
      SPINNING_CCW = -1,
      SPINNING_CW = 1,
      ATTACKING
    } iState;

    inline bool isActiveTactic(void) const
    {
      return true;
    }

    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param *tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10000;
      for (std::list<int>::const_iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        float dis_from_ball = (state->homePos[*it] - state->ballPos).absSq();
        if(dis_from_ball < mindis)
        {
          mindis = dis_from_ball;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot

    void execute(const Param& tParam)
    {
      Vector2D<int> goal(HALF_FIELD_MAXX, 0);
      float finalSlope = Vector2D<int>::angle(goal, state->homePos[botID]);
      float turnAngleLeft = normalizeAngle(finalSlope - state->homeAngle[botID]); // Angle left to turn
      float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
      if(dist > BOT_BALL_THRESH)
      {
        float finalSlope = Vector2D<int>::angle(Vector2D<int>(ForwardX(HALF_FIELD_MAXX, 0)), state->ballPos);
        captureBall();
        return;
      }
      if(fabs((float)turnAngleLeft) > 2 * SATISFIABLE_THETA)
      {
        turnToPoint(goal.x, goal.y);
        return;
      }
      //printf("should kick now %f\n", turnAngleLeft);
      kickBallForGoal();
      if (state->homePos[botID].absSq() < BOT_BALL_THRESH * BOT_BALL_THRESH)
      {
        tState = COMPLETED;
      }
    }
  }; // class TTStop
} // namespace MyStrategy

#endif // TTSTOP_HPP
