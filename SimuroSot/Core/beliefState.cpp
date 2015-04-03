#pragma once 
#include "stdafx.h"
#include <ctime>
#include "beliefState.h"
#include "../common/include/config.h"
#include "../common/include/thread.h"
#include "../common//include/cs.hpp"
#include "../common/include/config.h"
#include "../Skills/skillSet.h"
 #include "../winDebugger/Client.h"


using namespace std;
using namespace Util;
using namespace HAL;
using namespace Simulator;

static char debug[500];
#define print(...) {sprintf(debug,__VA_ARGS__); Client::debugClient->SendMessages(debug);}
namespace MyStrategy
{

  BeliefState::BeliefState() :
    prevFrameNum(-1),
    currFrameNum(-1)
  {
    for(int botID = 0; botID < HomeTeam::SIZE; botID++)
    {
      homeAngle[botID] = 0;
    }

    for(int botID = 0; botID < AwayTeam::SIZE; botID++)
    {
      awayAngle[botID] = 0;
    }
    ourGoalCount    = 0;
    oppGoalCount    = 0;
    timeRemaining   = 0;
    // If Refree Box is not going to run do not wait for signal from refree box to start the game
#ifdef RUN_REFBOX
    pr_gameRunning     = false;
#else
    pr_gameRunning     = true;
#endif
    pr_gameHalted      = false;
    pr_ourFreeKick     = false;
    pr_oppFreeKick     = false;
    pr_ourGoalKick     = false;
    pr_oppGoalKick     = false;
    pr_ourGoalScored = false;
    pr_oppGoalScored = false;
	ballOppGoal=false;
	ballOurGoal=false;
  }

  BeliefState::~BeliefState()
  {

  }

  bool BeliefState::update()
  {
    computeBallLocation();
    computeBallInDBox();
    computeBallOurSide();
    computeBallOppSide();
    computeBallInStrips();
	checkForGoal();
    return true;
  } // update
  /* Function definitions for all the predicates...
     * */

  void BeliefState::computeBallLocation()
  {
    int homeNearbotID = -1, awayNearbotID = -1;
    float minDist, distSq, closestBot ;
    minDist = 1e20;
    for(int i = 0; i < HomeTeam::SIZE ; i++)
    {
      float th = Vector2D<int>::angle(ballPos, homePos[i]);
      float theta = normalizeAngle(th - homeAngle[i]);
      distSq =  Vector2D<int>::distSq(ballPos , homePos[i]);

      if(distSq < minDist)
      {
        minDist = distSq;
        closestBot = i;
      }
      this->ourBotNearestToBall = closestBot;
      if(distSq < (BOT_BALL_THRESH_FOR_PR * BOT_BALL_THRESH_FOR_PR) && fabs((float)theta) < 2*DRIBBLER_BALL_ANGLE_RANGE)
      {
        homeNearbotID = i;
        break ;
      }
    }
    minDist = 1e20;
    for(int i = 0; i < AwayTeam::SIZE ; i++)
    {
      float theta = normalizeAngle(Vector2D<int>::angle(ballPos, homePos[i]) - awayAngle[i]);
      distSq = Vector2D<int>::distSq(ballPos , awayPos[i]);
      if(distSq < minDist)
      {
        minDist = distSq;
        closestBot = i;
      }
      this->oppBotNearestToBall = closestBot;
      if(distSq < (BOT_BALL_THRESH * BOT_BALL_THRESH) && fabs((float)theta) < DRIBBLER_BALL_ANGLE_RANGE)
      {
        awayNearbotID = i;
        break ;
      }
    }
    //printf("OurBotnearest, OppBotNearest : %d %d\n",ourBotNearestToBall,oppBotNearestToBall);
    if(awayNearbotID == -1 && homeNearbotID == -1)
    {
      pr_looseBall = true;
      pr_oppBall = false;
      pr_ourBall = false;

    }
    else if(awayNearbotID == -1 && homeNearbotID != -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  false;
      pr_ourBall    =  true;
    }
    else if(awayNearbotID != -1 && homeNearbotID == -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  true;
      pr_ourBall    =  false;
    }
    else if(awayNearbotID != -1 && homeNearbotID != -1)
    {
      pr_looseBall  =  false;
      pr_oppBall    =  true;
      pr_ourBall    =  false;

      //TODO: Incorporate bot orientation to decide if homeBot is pointing in the right direction
    }

  }

  void BeliefState::computeBallInDBox()
  {
    pr_ball_in_opp_dbox = false;
    pr_ball_in_our_dbox = false;

    if(ballPos.x < (-HALF_FIELD_MAXX + DBOX_WIDTH) &&
        ballPos.y < OUR_GOAL_MAXY &&
        ballPos.y > OUR_GOAL_MINY)
      pr_ball_in_our_dbox = true;
    else if(ballPos.x > (HALF_FIELD_MAXX - DBOX_WIDTH) &&
            ballPos.y < OUR_GOAL_MAXY &&
            ballPos.y > OUR_GOAL_MINY)
      pr_ball_in_opp_dbox = true;
  }

  //The following three assume that 0,0 is at center of frame (for checking ball parameters)

  void BeliefState::computeBallOppSide()
  {
    if((ballPos.x) > 0)
    {
      //MID_FIELD_THRESH is the distance from midfield line into which when ball goes ball's side changes
      if((ballPos.x > fieldCentre.x + MID_FIELD_THRESH) && (pr_ourBall == true))
        pr_ballOppSide = true ;
      //check ball VELOCITY
      else if((ballVel.x) * (opp_goalpoints[2].x - fieldCentre.x) > 0) //opp_goalpoints[2] is mid point of goal
        pr_ballOppSide = true ;
      else
        pr_ballOppSide = false;
    }
  }

  void BeliefState::computeBallOurSide()
  {
    if(ballPos.x < fieldCentre.x)
    {
      if((ballPos.x < fieldCentre.x - MID_FIELD_THRESH) && (pr_oppBall == true)) //MID_FIELD_THRESH is the distance from midfield line into which when ball goes ball's side changes
      { pr_ballOurSide = true ; pr_ballOppSide =false;}
    
    }
  }

  void BeliefState::computeBallMidfield()
  {

    if((pr_ballOppSide != true) && (pr_ballOurSide != true))
      pr_ballMidField = true ;
  }

  bool BeliefState::pr_nOpponentsOurSide(int n)
  {
    int ctr = 0;
    for(int i = 0; i < AwayTeam::SIZE ; i++)
    {
      if(awayPos[i].x < fieldCentre.x)
        ctr ++ ;
    }
    if(ctr == n)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballXGt(int x)
  {
    if(ballPos.x > x)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballXLt(int x)
  {
    if(ballPos.x  < x)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballAbsYGt(int y)
  {
    int absy = ballPos.y > 0 ? ballPos.y : (-(ballPos.y)) ;
    if(absy > y)
      return true ;
    else
      return false ;
  }

  bool BeliefState::pr_ballAbsYLt(int y)
  {
    int absy = ballPos.y > 0 ? ballPos.y : (-(ballPos.y)) ;
    if(absy < y)
      return true ;
    else
      return false ;
  }

  void BeliefState::computeBallInStrips()
  {
    pr_ballInFrontStrip = (ballPos.x > HALF_FIELD_MAXX - STRIP_WIDTH_X || ballPos.x < -HALF_FIELD_MAXX + STRIP_WIDTH_X);
    pr_ballInSideStrip  = (ballPos.y > HALF_FIELD_MAXY - STRIP_WIDTH_Y || ballPos.y < -HALF_FIELD_MAXY + STRIP_WIDTH_Y);
  }

 
  int BeliefState::ourBotNearestToPoint(const int x, const int y) {
    float minDis  = 1e20;
    int   botId   = -1;
    for(int i = 0; i < HomeTeam::SIZE; i++) {
      float dnew = Vector2D<int>::dist(homePos[i], x, y);
      if(dnew < minDis) {
        minDis = dnew;
        botId  = i;
      }
    }
    return botId;
  }
  
  int BeliefState::oppBotNearestToPoint(const int x, const int y) {
    float minDis  = 1e20;
    int   botId   = -1;
    for(int i = 0; i < AwayTeam::SIZE; i++) {
      float dnew = Vector2D<int>::dist(awayPos[i], x, y);
      if(dnew < minDis) {
        minDis = dnew;
        botId  = i;
      }
    }
    return botId;
  }

  void BeliefState::checkForGoal(){
	//  print("oppGoal Scored is %d ballpos-%d %d",oppGoalCount, ballPos.x, ballPos.y);
	 // print("ourGoal Scored is %d ballPos-%d %d",ourGoalCount, ballPos.x, ballPos.y);
	  if(ballOppGoal==true){
		  if(ballPos.x < HALF_FIELD_MAXX/2)
			  ballOppGoal=false;
		      return;
	  }
	  else if(ballOurGoal==true){
		  if(ballPos.x > -HALF_FIELD_MAXX/2)
			  ballOurGoal=false;
		      return;
	  }
	  if(ballPos.x <= HALF_FIELD_MAXX-GOAL_DEPTH && ballPos.x >= -HALF_FIELD_MAXX+GOAL_DEPTH){
		  ballOppGoal=false;
		  ballOurGoal=false;
		  return;
	  }
	  if(ballPos.x >HALF_FIELD_MAXX-GOAL_DEPTH && ballPos.y<OPP_GOAL_MAXY && ballPos.y >OPP_GOAL_MINY){
		  if(ballOppGoal==false){
		       ourGoalCount++;
		//	   print("oppGoal Scored is %d",oppGoalCount);
		  }
		  ballOppGoal=true;
	  }
	  else if(ballPos.x < -HALF_FIELD_MAXX+GOAL_DEPTH && ballPos.y < OUR_GOAL_MAXY && ballPos.y > OUR_GOAL_MINY){
		  if(ballOurGoal==false){
		       oppGoalCount++;
			   print("ourGoal Scored is %d",ourGoalCount);
		  }
		  ballOurGoal=true;
	  }
  }
} // namespace MyStrategy