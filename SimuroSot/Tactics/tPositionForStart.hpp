#ifndef TPOSITIONFORSTART_HPP
#define TPOSITIONFORSTART_HPP
#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"
namespace MyStrategy
{
  class TPositionForStart : public Tactic
  {
  public:
    TPositionForStart(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    { } // TPositionForStart
    ~TPositionForStart()
    { } // ~TPositionForStart
    inline bool isActiveTactic(void) const
    {
      return false;
    }
//CHOOSbEST bOT AND the giving of parameters for going to the required point needs to be entered
//The cordinates are hard-coded for the starting of the game....in terms of the actual measurements of the field given. The hard code is in mm
    /*Home team is starting...
    bot0:Goalkeeper                                                                      x=1300, y=900;
    bot1 :Positioned at centre.                                                          x=0,   y=0;
    bot2 : Centre of midield and goal                                                    x= 650, y=900;
    bot3,bot4 :on the radius of the circle of the centre circle on our goal side         x= -220 y1=125;
                                                                                         x=-220  y2=-125;
     Away team is starting
    bot0:Goalkeeper                                                                       x=1300, y=900;
    bot1:Centre of midfield and goal...                                                    x= 650, y=900;
    bot2,3,4 : Around the centre of the circle                                             x= -250,y=0;
     *                                                                                     x=-180,y=180;
                                                                                           x=-180,y=180;
     */
    int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const
    {
      int minv = *(freeBots.begin());
      int mindis = 10;
      for (std::list<int>::iterator it = freeBots.begin(); it != freeBots.end(); ++it)
      {
        // TODO make the bot choosing process more sophisticated, the logic below returns the 1st available bot
        //this tactic does not require too much of sophistication for the tactic and the bot which is at the minimum  distance from the point is the best bot to do the job...
        float dis_from_point = (state->homePos[*it] - state->ballPos).absSq();
        if(dis_from_point < mindis)
        {
          dis_from_point = mindis;
          minv = *it;
        }
      }
      return minv;
    } // chooseBestBot
    void execute(const Param& tParam)
    {
      // Select the skill to the executed next
      gotoPointExact(tParam.PositionForStartP.x, tParam.PositionForPenaltyP.y, true, tParam.PositionForStartP.finalSlope, tParam.PositionForStartP.finalVelocity);
      if (state->homeVel[botID].absSq() < BOT_POINT_THRESH * BOT_POINT_THRESH)
      {
        tState = COMPLETED;
      }
    }
  }; // class TPositionForStart
} // namespace MyStrategy
#endif // TPOSITIONFORSTART_HPP
