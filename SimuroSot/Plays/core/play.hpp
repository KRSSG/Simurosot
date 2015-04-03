#ifndef PLAY_HPP
#define PLAY_HPP

#include <utility>
#include <vector>
#include <string>
#include "../../Core/beliefState.h"
#include "../../Tactics/robot.h"
#include "../../common/include/timer.h"

namespace MyStrategy
{
  class Play
  {
    friend class PS;

  public:
    enum Result
    {
      SUCCESS_LOW,//0
      SUCCESS_MED,//1
      SUCCESS_HIGH,//2
      FAILURE_LOW,//3
      FAILURE_MED,//4
      FAILURE_HIGH,//5
      COMPLETED,//6
      TIMED_OUT,//7
      NOT_TERMINATED//8
    }; // enum Result
    inline bool timedOut()
    {
      //printf("Time left: %d\n", timer.split() - timeoutPeriod);
      int tm = timer.split();
      if((tm - lastRoleReassignTime) > roleReassignPeriod) {
        lastRoleReassignTime = tm;
        shouldRolesReassign  = true;
      }
      return (tm >= timeoutPeriod);
    }
    enum PlayType
    {
      PLAYTYPE_YES,
      PLAYTYPE_NO,
      PLAYTYPE_DONTCARE
    };
  private:
    // Default timeout period of any play
    static const int DEFAULT_TIMEOUT_PERIOD = 60;

    Util::Timer timer;
    int         timeoutPeriod; // in milliseconds
    int         roleReassignPeriod;
    int         lastRoleReassignTime;

    // Sets the timeout period for the play in seconds
    inline void startTimer(void)
    {
      timer.start();
			//int tm = timer.split();
      lastRoleReassignTime =0;// tm - 2*roleReassignPeriod; //so that 1st call to timedOut gives shouldRolesReassign = true. 2* just to be safe
    }

  protected:
    Result             result;  // Assigned when the play terminates
    const BeliefState& state;

    // Sets the timeout period of the play in seconds
    void setTimeout(int timeoutPeriod, int roleReassignPeriod = 1)
    {
      this->timeoutPeriod = (timeoutPeriod * 1000);
      this->roleReassignPeriod = roleReassignPeriod * 1000;
    }

    inline void computeMaxTacticTransits()
    {
      maxTacticsPerRole = roleList[0].size();
      for (int roleIdx = 1; roleIdx < HomeTeam::SIZE; ++roleIdx)
      {
        int sz = roleList[roleIdx].size();
        if (roleList[roleIdx].size() > maxTacticsPerRole)
        {
          maxTacticsPerRole = sz;
        }
      }
    }

	public:
   
   //search for opponent bot if its at desireed postion #positional plays
 bool search_opp_bot(const BeliefState* state,Vector2D<int> dpoint) 
 {
	  int i = 0 ;
	  float dist1 ;
  if((abs(dpoint.x)>HALF_FIELD_MAXX)||(abs(dpoint.y)>HALF_FIELD_MAXY))
	  return true ;
 
 while(i<5)
   {
	 dist1 = Vector2D<int>::dist(dpoint,state->awayPos[i]);
	 if(dist1< 2*BOT_BALL_THRESH)
		 return true;
     i++;   
   }
  return false;	
	  
  }
	
//provides correct point
bool position_in_dbox(Vector2D<int> dpoint)
{
if(((abs(dpoint.x)<HALF_FIELD_MAXX)&&(abs(dpoint.x)>(HALF_FIELD_MAXX - DBOX_WIDTH)))&&(abs(dpoint.y)<DBOX_HEIGHT))	
  return true ;
else 
  return false ;  
}

Vector2D<int> position_correcter(const BeliefState* state,Vector2D<int> dpoint)
{
float dist ;
 if((abs(dpoint.x)>HALF_FIELD_MAXX)&&(abs(dpoint.y)>HALF_FIELD_MAXY))	
  {
	if(abs(dpoint.x)>HALF_FIELD_MAXX)
	  dpoint.x = SGN(dpoint.x)*HALF_FIELD_MAXX ;
	
	if(abs(dpoint.y)>HALF_FIELD_MAXY)
		dpoint.y = SGN(dpoint.y)*HALF_FIELD_MAXY ;
	
  }
  
    /*if(position_in_dbox(dpoint))
	{
	   dpoint.x = SGN(dpoint.x)*(HALF_FIELD_MAXX- DBOX_WIDTH) ;
	}
    */
	return dpoint;
  
}	
	
	
 //porvides the hassle free point
 Vector2D<int> position_our_bot(const BeliefState* state,Vector2D<int> dpoint,float KICK_THRESH)
 {
  float dist;
  Vector2D<int> point;
  int i =0 ;
  if((!search_opp_bot(state,dpoint))&&((Vector2D<int>::dist(dpoint,state->ballPos))>KICK_THRESH))
   return dpoint ;
  
  while(i<4)
  {
	 if(search_opp_bot(state,dpoint))
     {
	   switch(i)
       {  case 0 : 
	      point.x = dpoint.x + 2*BOT_RADIUS ; point.y =  dpoint.y ;
		  if((abs(point.x)<HALF_FIELD_MAXX)&&(abs(point.x - state->ballPos.x)>KICK_THRESH))
			  dpoint = point ;
	      break;
		  
		  case 1 :
		  point.x = dpoint.x - 2*BOT_RADIUS ; point.y =  dpoint.y ;
		  if((abs(point.x)<HALF_FIELD_MAXX)&&(abs(point.x - state->ballPos.x)>KICK_THRESH))
			  dpoint = point ;
          break;
		  
		  case 2 :
		  point.x = dpoint.x ; point.y =  dpoint.y + 2*BOT_RADIUS ;
		  if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
			  dpoint = point ;
		  break;
		  
		  case 3 :
           point.x = dpoint.x ; point.y =  dpoint.y - 2*BOT_RADIUS ;
		   if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
			   dpoint = point ;
		   break;
	   }
	   if(i==3)
		 i = 0;
     
     i++;	   
     }
     else
      return dpoint ;
 
  }
 }
 //using queue(BSF)
  /*
  queue < Vector2D<int> > vpoint;
  vpoint.push(dpoint);
  Vector2D<int> point = dpoint;
  while(vpoint.size()!=0)
  {	
   if(search_opp_bot(state,vpoint.front()))
    {
	  dpoint = vpoint.front();
	  point.x = dpoint.x + 2*BOT_RADIUS ; point.y =  dpoint.y ;
	  if((abs(dpoint.x)<HALF_FIELD_MAXX)&&(abs(dpoint.x - state->ballPos.x)>KICK_THRESH))
		vpoint.push(point);
	  point.x = dpoint.x - 2*BOT_RADIUS ; point.y =  dpoint.y;
	  if((abs(point.x)<HALF_FIELD_MAXX)&&(abs(point.x - state->ballPos.x)>KICK_THRESH))
		vpoint.push(point); 
      point.x = dpoint.x ; point.y =  dpoint.y + 2*BOT_RADIUS ;
      if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
		vpoint.push(point);    
	  point.x = dpoint.x ; point.y =  dpoint.y - 2*BOT_RADIUS;
      if((abs(point.y)<HALF_FIELD_MAXY)&&(abs(point.y - state->ballPos.y)>KICK_THRESH))
		vpoint.push(point);    	
		
		vpoint.pop();
     }
    else
	{ 
	  return vpoint.front();
	  break;
	}
  }
}	
   */

	
    bool        shouldRolesReassign;
	virtual void reevaluateRoleParams() {} //Function to reevaluate the params for each role in every call of executePlay() in pExec
    inline Play(const BeliefState& state) :
      timeoutPeriod(Play::DEFAULT_TIMEOUT_PERIOD),
      result(NOT_TERMINATED),
      state(state),
      weight(1.0f),
      maxTacticsPerRole(0)
    {
      name = "";
      setTimeout(60);
    }

    // Name of the play
    std::string name;
    PlayType PositionPlay;
    PlayType AttackPlay;
    
    // Weight of the play - Measure of its applicability in the current scenario
    float weight;

    // roleList is a vector of pairs of Tactic ID and Tactic Parameter
    std::vector<std::pair<Tactic::ID, Tactic::Param> > roleList[HomeTeam::SIZE];
    int assignedBot[HomeTeam::SIZE];
    unsigned int maxTacticsPerRole;

    // Returns true if the play is applicable otherwise false
    virtual bool applicable(void) const {
      if((state.pr_gameRunning && this->PositionPlay == PLAYTYPE_YES)|| (!state.pr_gameRunning && this->PositionPlay == PLAYTYPE_NO)) {
        //printf("%d %d", state.gameRunning, this->PositionPlay);
        return false;
      }
      return true;
    }

    /* Returns one of SUCCESS/FAILURE/ABORTED/COMPLETED as applicable if the play terminates
     * otherwise NO_TERMINATION
     */
    virtual Result done(void) const = 0;
  }; // class Play
} // namespace Strategy

#endif // PLAY_HPP