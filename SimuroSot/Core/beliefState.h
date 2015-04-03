#pragma once
#ifndef BELIEF_STATE_H
#define BELIEF_STATE_H

#include "..\common\include\geometry.hpp"
#include "..\Utils\comdef.h"
// Forward Declarations
namespace HAL
{
  class VisionInfo;
  class RefBoxCmd;
}

namespace MyStrategy
{
  class VisionThread;
}

namespace Util
{
  class CS;
}

namespace MyStrategy
{
  class BeliefState
  {
  private:
    int              prevFrameNum;
    // stateSh is the shared belief state. It will be shared by the home robots.
  public:
    BeliefState();

    ~BeliefState();

    // It tries to update the belief state. If it does not change, the function returns false else true
    bool      update();
    int       currFrameNum;

    Point2D<int>    homePos[HomeTeam::SIZE];
    Point2D<int>    fieldCentre;
    Point2D<int>    home_goalpoints[5];
    Point2D<int>    opp_goalpoints[5];
    Vector2D<float> homeVel[HomeTeam::SIZE];
    Vector2D<float> homeAcc[HomeTeam::SIZE];
    float           homeAngle[HomeTeam::SIZE];
    float           homeOmega[HomeTeam::SIZE];
    float           homeAngAcc[HomeTeam::SIZE];
    
    Point2D<int>    awayPos[HomeTeam::SIZE];
    Vector2D<float> awayVel[HomeTeam::SIZE];
    Vector2D<float> awayAcc[HomeTeam::SIZE];
    float           awayAngle[HomeTeam::SIZE];
    float           awayOmega[HomeTeam::SIZE];
    float           awayAngAcc[HomeTeam::SIZE];
    
    Point2D<int>    ballPos;
    Vector2D<float> ballVel;
    Vector2D<float> ballAcc;

    // Rarely changing elements from refree box
    int ourGoalCount;
    int oppGoalCount;
    int timeRemaining;
    bool pr_gameHalted;                /* This state is necessary to select the play to halt the robots in their position*/
    bool pr_gameRunning;
    bool pr_ourFreeKick;
    bool pr_oppFreeKick;
    bool pr_ourGoalKick;
    bool pr_oppGoalKick;
    bool pr_ourPenaltyKick;
    bool pr_oppPenaltyKick;
    bool ballOppGoal;
	bool ballOurGoal;
    //------- Set of high-level predicates -------//
    
    /* This predicates are calculates by computeBallLocation */
    int ourBotNearestToBall;
    int oppBotNearestToBall;
    
    /* The two predicates below are computed in computeBallInDBox() */
    bool pr_ball_in_our_dbox;
    bool pr_ball_in_opp_dbox;
    
    // TODO Unimplemented
    
    //@predicate for goal scored
    bool pr_ourGoalScored;
    bool pr_oppGoalScored;

    // @predicate offense
    bool pr_offense;

    // @predicate defense
    bool pr_defense;

    // @predicate oppBall
    bool pr_oppBall;

    // @predicate ourBall
    bool pr_ourBall;

    // @predicate looseBall
    bool pr_looseBall;

    // @predicate ballOppSide
    bool pr_ballOppSide;

    // @predicate ballOurSide
    bool pr_ballOurSide;

    // @predicate ballMidField
    bool pr_ballMidField;

    // @predicate ballInOurCorner
    bool pr_balInOurCorner;

    // @predicate ballInOppCorner
    bool pr_ballInOppCorner;

    // @predicate ourKickOff
    bool pr_ourKickOff;

    // @predicate oppKickOff
    bool pr_oppKickOff;
 
    // Predicates that take an input parameter
    // @predicate nOpponentsOurSide < n >
    bool pr_nOpponentsOurSide(int n);

    // @predicate ballXGt < x >
    bool pr_ballXGt(int x);

    // @predicate ballXLt < x >
    bool pr_ballXLt(int x);

    // @predicate ballAbsYGt < y >
    bool pr_ballAbsYGt(int y);

    // @predicate ballAbsYLt < y >
    bool pr_ballAbsYLt(int y);

    // @predicate ballInSideStrip
    bool pr_ballInSideStrip;
    
    // @predicate ballInFrontStrip
    bool pr_ballInFrontStrip;
    
    // Prototypes for functions that compute the values of the predicates
    void computeBallInDBox();
    void computeOffense();
    void computeDefense();
    
    void computeBallLocation();
    void computeBallOppSide();
    void computeBallOurSide();
    void computeBallMidfield();

    void computeBallInOurCorner();
    void computeBallInOppCorner();

    void computeOurKickOff();
    void computeOppKickOff();

    void computeOurFreeKick();
    void computeOppFreeKick();

    void computeOurPenalty();
    void computeOppPenalty();
    
    void computeBallInStrips();
    
	void checkForGoal();
    // Functional Predicates
    int ourBotNearestToPoint(const int x, const int y);
    int oppBotNearestToPoint(const int x, const int y);
  };   // class BeliefState
} // namespace MyStrategy

#endif // BELIEF_STATE_H