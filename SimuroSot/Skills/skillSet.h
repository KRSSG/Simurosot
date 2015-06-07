// It is through the skill layer that the MyStrategy layer talks to the Simulator/Actual robots
#pragma once
#ifndef SKILLS_H
#define SKILLS_H
#include "..\common\dlib\dlib\svm.h"
#include "../HAL/comm.h"
#include <vector>
#include "..\Utils\pathPlanners.h"
// Forward Declarations
namespace MyStrategy
{
  class MergeSCurve;
  class LocalAvoidance;
  class BeliefState;
}

template <class T> class Vector2D;
namespace MyStrategy
{
  /* Procedure for adding a skill successfully to the SkillSet is:
   * 1. Enumarate it in enum SkillSet::SkillID just before MAX_SKILLS
   * 2. Identify its input parameters and define a struct for it in union SParam
   * 3. Declare the function prototype for the skill. Its parameter must only be SParam& param
   * 4. Map the Skill ID to the corresponding function in the constructor of SkillSet
   * 5. Define the function that implements the skill declared in step 3.
   * NOTE: Variable names of enums and corresponding skill function names must be the same,
   *       except that the enum name starts with a capital letter. Name of the corresponding
   *       struct should be the same as that of enum with a trailing 'P' character
   */
  // Union of parameters for each skill enumerated in SkillID
    union SParam
    {
      // Parameters for the skill Spin
      struct type1
      {
        float radPerSec;
      } SpinP;

      // Parameters for the skill Stop
      struct type2
      {} StopP, GoalKeepingP,ChargGoal;

      // Parameters for the skill Kick
      struct type3
      {
        float power;
      } KickP;

      // Parameters for the skill Velocity
      struct type4
      {
        float vl;
        float vr;
      } VelocityP;

      //Parameters for skill goToPoint and dribble to point
      struct type5
      {
        float x;
        float y;
        float finalslope;
      } DribbleToPointP, TurnToPointP, DefendPointP,GoToPointDWP;
      struct type6
      {
        float finalslope;
      } TurnToAngleP;
      struct type7
      {
        bool align;
      } GoToBallP;
      struct type8
      {
        float x;
        float y;
        float finalslope;
        bool align;
        float finalVelocity;
		//Parameters for learning?? not needed
		float bot_point_dist_penalty;
		float angle_diff_penalty;
        bool increaseSpeed;
		bool trapezoidal;
      } GoToPointP;
	  //struct type9
	  //{
		 // //*************** trying to use DYNAMIC WINDOW for kicking  ........ *****gunjan ***********
		
	  //}KickBallP;

    };
  class SkillSet
  {
  public:
    enum SkillID
    {
      Spin,
      Stop,
      Velocity,
      GoToBall,
      GoToPoint,
	  GoToPointDW,
      GoalKeeping,
      TurnToPoint,
      TurnToAngle,
      DefendPoint,
      GoToPointStraight,
      GoToBallStraight,
	  ChargeBall,
	 // KickBall,
      MAX_SKILLS
    };
    static Comm*    comm;
	 protected:
    typedef void (SkillSet::*skillFncPtr)(const SParam&);
    const BeliefState* state;
    const int          botID;
    skillFncPtr        skillList[MAX_SKILLS];
#ifdef LOCAL_AVOID
    LocalAvoidance*    pathPlanner;
    MergeSCurve*       pathPlannerSec;
#else
    MergeSCurve*       pathPlanner;
#endif
    
    /* TODO: Extra Shit, relevance needs to be verified
     */
    bool pointyInField(Vector2D<int> fianl);
    void _goToPoint(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance,bool increaseSpeed=0,bool trapz = false);
    void _goToPointDW(int botid, Vector2D<int> dpoint, float finalvel, float finalslope);
	void _goToPointPolar(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance,bool increaseSpeed=0,bool trapz = false);
	    void _goToPointLessThresh(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance,bool increaseSpeed=0);
    void _dribbleToPoint(int botID, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance);
    void _turnToAngle(float angle, float *vl, float *vr);
    void _goToPointStraight(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance);
    int prevVel;
    /***************************By Prasann***********/
    float predicted_distance;
    float predicted_rotation;
    Point2D<int> prevpoint;
    float prevorientation;
    float errordistance;
    float errororientation;
    /***************************By Prasann***********/
  public:
    //------- List of robot skills -------//
    void spin(const SParam& param);
    void stop(const SParam& param);
    //void kick(const SParam& param);
    //void dribble(const SParam& param);
    void velocity(const SParam& param);
    void goToBall(const SParam& param);
    void goToPoint(const SParam& param);
	void goToPointDW(const SParam& param);
    void goalKeeping(const SParam& param);
    void turnToPoint(const SParam& param);
    void turnToAngle(const SParam& param);
    void defendPoint(const SParam& param);
    void goToPointStraight(const SParam& param);
    void goToBallStraight(const SParam& param);
	void chargeBall(const SParam& param);
    void kickBall(const SParam& param);

    // Parameter for skills to be trained
    static bool loadParamsFromFile;
    static bool  skillParamsLoaded;
    static float GoToPoint_profileFactorMult;
    static float GoToPoint_omegaFactor;
    static float Bot_Point_dis_penalty_l1;
    static float Bot_Point_dis_penalty_l2;
    static float angle_penalty_l1;
    static float angle_penalty_l2;
    static dlib::decision_function<dlib::radial_basis_kernel<dlib::matrix<double, 3, 1> > > gotopoint_profilefac;
    static dlib::decision_function<dlib::radial_basis_kernel<dlib::matrix<double, 3, 1> > > gotopoint_omegafac;
    static int dribble_ball_threshold;
    static int bot_ball_threshold;
    static std::string skillsCollection;
    static std::string goToPointCollection;
    /* Error Function by
     * Soumyadeep & Ankit
     */
    void errorLog(int botid,float translation,float rotation);
    
    
    SkillSet(const BeliefState* state, int botID);
    ~SkillSet();
    // Executing the skill function corresponding to the given ID and param function parameters
    inline void executeSkill(SkillID ID, const SParam& param)
    {
      (*this.*skillList[ID])(param);
    } // executeSkill
  }; // class SkillSet
} // namespace MyStrategy
#endif // SKILLS_H
