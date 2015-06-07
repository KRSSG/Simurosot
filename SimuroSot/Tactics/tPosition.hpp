#ifndef TPOSITION_HPP
#define TPOSITION_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "../winDebugger/Client.h"
#include "comdef.h"
#include <vector>
using namespace std;
namespace MyStrategy
{
  class TPosition: public Tactic
  {
  public:

    TPosition(const BeliefState* state, int botID) :
      Tactic(Tactic::Stop, state, botID)
    {

    } // TPosition

    ~TPosition()
    { } // ~TPosition
    inline bool isActiveTactic(void) const
    {
      return false;
    }
    //CHOOSEbEST bOT AND the giving of parameters for going to the required point needs to be entered
    //Choose best bot also needs to get the params that the tactic has in order to choose the best bot....


    void execute(const Param& tParam)
    {
		
      vector<pair<float, float> > obstacles;
      Vector2D<int> oppGoalCenter(ForwardX(HALF_FIELD_MAXX), 0);
      getFullAngularClearance(obstacles, Vector2D<int>::dist(state->homePos[botID], oppGoalCenter)-1, state->homePos[botID]);
	
      //cout<<obstacles.size()<<endl;
      /*for(vector<pair<float, float> >::iterator it = obstacles.begin(); it != obstacles.end(); it++) {
        SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y, 
                    state->homePos[botID].x + 10000*cos(it->first), state->homePos[botID].y + 10000*sin(it->first), 0xFF0000);
        SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y, 
                    state->homePos[botID].x + 10000*cos(it->second), state->homePos[botID].y + 10000*sin(it->second), 0x00FF00);
      }*/
	 
   //   pair<float, float> res;
    //  getOppGoalClearance(obstacles, state->homePos[botID], res);
    //  //Client::debugClient->SendMessages("Am I here 7\n");
      // Select the skill to the executed next
      gotoPointExact(tParam.PositionP.x, tParam.PositionP.y, tParam.PositionP.align, tParam.PositionP.finalSlope, tParam.PositionP.finalVelocity);
 
	}
  }; // class TPosition
} // namespace MyStrategy

#endif // TPOSITION_HPP
