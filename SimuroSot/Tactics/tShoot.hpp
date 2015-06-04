#ifndef TSHOOT_HPP
#define TSHOOT_HPP
#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
namespace MyStrategy
{
  class TShoot : public Tactic
  {
  public:
    TShoot(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TShoot
    ~TShoot()
    { } // ~TShoot
    inline bool isActiveTactic(void) const
    {
      return true;
    }

    void execute(const Param& tParam)
    {
      float tmp_angle1, tmp_angle2, tmp_angle3;
      float angle1, angle2, finalSlope;
      tmp_angle1 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX, OPP_GOAL_MAXY), state->ballPos);
      tmp_angle2 = Vector2D<int>::angle(getOppGoalie(), state->ballPos);
      tmp_angle3 = Vector2D<int>::angle(Vector2D<int>(HALF_FIELD_MAXX, OPP_GOAL_MINY), state->ballPos);
      angle1 = fabs((float)tmp_angle2 - tmp_angle1);
      angle2 = fabs((float)tmp_angle3 - tmp_angle2);
      if(angle1 > angle2)
        finalSlope = (tmp_angle2 + tmp_angle1) / 2;
      else
        finalSlope = (tmp_angle2 + tmp_angle3) / 2;
     // Util::// LoggertoStdOut("Final Angle : %f\n", finalSlope);
      float dist_from_ball = Vector2D<int>::dist(state->homePos[botID], state->ballPos);
      int finalx, finaly;
      if(dist_from_ball < BOT_BALL_THRESH)
      {
        finalx = HALF_FIELD_MAXX;
        /* The point of intersection with the goal Vertical line y = mx1 + (y2 - mx2)*/
        finaly = finalSlope * HALF_FIELD_MAXX + (state->homePos[botID].y - finalSlope * state->homePos[botID].x);
      }
      else
      {
        finalx = state->ballPos.x;
        finaly = state->ballPos.y;
      }
      gotoPointExact(finalx, finaly, true, finalSlope, 0);
    }
    Vector2D<int> getOppGoalie()
    {
      return state->awayPos[0];
    }
  }; // class TShoot
} // namespace MyStrategy
#endif // TSHOOT_HPP
