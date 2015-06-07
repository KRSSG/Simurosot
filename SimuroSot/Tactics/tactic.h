#ifndef TACTIC_H
#define TACTIC_H
#include <list>
#include "..\Skills\skillSet.h"
#include <cstring>
#include "..\HAL\comm.h"
// Forward Declarations
namespace MyStrategy
{
  class BeliefState;
}
/*namespace HAL
{
  class Comm;
}*/
namespace MyStrategy
{
  class Tactic
  {
    friend class PExec;
  public:
    enum ID
    {
      Block = 0,            // Defend the goal a particular distance from the goal
      ChargeBall,           // Set-play tactic
      CoverGoal,
	  DragToGoal,           //Go to the ball and start dragging the ballto goal
      DefendLine,           // Defend the ball from crossing a particular line on the field
      DefendPoint,          // Defend the ball from a particular point on the field
      GoalieOur,            // A special tactic just for the goalie for our side
      GoalieOpp,            // Grant Goalie to opponent team. only for testing purpose.
      MarkBot,              // Mark an opponent preventing them from getting the ball, getting to the goal, blocking a shot
      MidField,
	  //Pass,                 // Pass to a point
      Backup,
      Position,             // Go to the given position
      PositionForPenalty,   // Set-play tactic
     // PositionForReceive,   // Pass Receive check
      PositionForStart,     // Set-play tactic
      ReceiveBall,          // Receive Pass
      Defend,               // Defend the ball from coming to our side
      Attack,               // Attck the ball towards the goal      
      Steal,                // Manipulate the ball to remove possession of it from another robot*/
      Stop,                 // Stop the bot
      Velocity,             // Move at a fixed velocity     
      AttackLingo,
	  ChargeToGoal,
	  //Attacksupport,
      MAX_TACTICS,          // Total number of tactics. All Implemented Tactics above this point
    };
    /* List the UnImplented Tactics below this line. When Implemented:
       * 1. Put it above MAX_TACTIC
       * 2. Include tactic in robot.cpp
       * 3. Allocate an object for it in tacticList in Robot::Robot
       */

    union Param
    {
      /* Union of parameters for each tactic enumerated in Tactic::ID
       * Creating object initializes each member by default to zero
       */
            // Parameters for tactic Goalie
      struct type1
	  { } GoalieP, ClearP, StealP, ShootP, StopP,MidFieldP;

      // Parameters for the tactic Block
      struct type2
      {
        float dist;   // Distance from the goal where the bot has to defend the goal
      } BlockP;

      // Parameters for the tactic Velocity
      struct type3
      {
        float vl ;
        float vr ;
		void init(float _vl, float _vr) {
          vl = _vl;
		  vr = _vr;
        }
      } VelocityP;
      // Parameters for side to defend (-1 for right and 1 for left )
      struct type4
      {
        int side;
      } DefendSideP;

      struct type5
      {
        float x ;
        float y ;
        float finalSlope ;
        float finalVelocity ;
        bool align;
		void init(float _x, float _y, float _finalSlope, float _finalVelocity, bool _align) {
          x = _x;
          y = _y;
          finalSlope = _finalSlope;
          finalVelocity = _finalVelocity;
          align = _align;
        }
      } PositionP , PositionForStartP, PositionForPenaltyP;

      struct type6
      {
        int awayBotID ;
      } MarkBotP;

      struct type7
      {
        int x1, x2, y1, y2;
      } DefendLineP;

      struct type8
      {
        int x, y,radius;
		void init(int _x, int _y) { x = _x; y = _y;}
      } DefendPointP;

      struct type9
      {
        int x;
      } DefendP;
      struct type10
      {
        bool rotateOnError;
	  } AttackP,AttackLingoP,AttackSupportP;      
      struct type11
      {
        int distFromGoal;
      } CoverGoalP; //tactic needs to be renamed to TDefendGoal
      Param()
      {
        /** Create a Parameter Object for Tactics
         * Initializes all the members initially to zero by default
         */
        memset(this, 0, sizeof(Param) );
      }
      Param(Tactic::ID tacticID)
      {
        /** Create a Parameter Object for Tactics
         * Initializes all the members initially to zero by default
         */
        memset(this, 0, sizeof(Param) );
        switch(tacticID)
        {
          case Tactic::Block:
            this->BlockP.dist = 100;
            break;
          default:
            break;
        }
      }
    };
    enum TState
    {
      INIT,                            // True when the tactic has just started else false
      RUNNING,                         // True when the tactic is in the middle of execution else false
      COMPLETED,                        // True when the tactic execution is completed else false
      ABORTED
    };
  protected:
    const Tactic::ID   tID;            // ID of the tactic to be executed
    const BeliefState* state;          // Belief State of the game
    const int          botID;          // ID of the bot executing this tactic
    SkillSet::SkillID   sID;            // ID of the skill to be executed
    SParam   sParam;                    // Parameters of the selected skill to be executed
    SkillSet*           skillSet;       // SkillSet object for the robot
  public:
    inline Tactic(ID                 tID,
                  const BeliefState* state,
                  int                botID) :
      tID(tID),
      state(state),
      botID(botID),
      sID(SkillSet::Stop),
      tState(INIT)
      //counter(0)
    {
      skillSet = new SkillSet(state, botID);
      tState = RUNNING;
    } // Tactic
    inline ~Tactic()
    {
      delete skillSet;
    } // ~Tactic
    
    Tactic::TState      tState;         // Current state of the tactic
    // True if the tactic is active, i.e., it involves ball manipulation
    virtual bool isActiveTactic(void) const = 0;
    inline void initialize(void)
    {
      tState = INIT;
    }
    // TODO As per the design, this function must be static but it cannot be both virtual as well as static
    /* This function assigns a score to all the free bots (bots that have not been
     * assigned any role yet) depending on how suitable they are in being assigned
     * the tactic. It then chooses the bot with the highest score and assigns it the tactic.
     */
    virtual int chooseBestBot(std::list<int>& freeBots, const Tactic::Param* tParam) const = 0;
    /* This function takes in the current tactic parameter for a robot and
     * using the belief state info and the skill transition rules,
     * it either decides to transit to another skill or continues to run
     * the current skill.
     */
    virtual void execute(const Param& tParam) = 0;
    // List utility functions for use in common in all tactics here.
    bool isBallInMyWideAngleRange() const;
    bool isBallInMyWideAngleRange(const int botID) const;
    bool isBallInMyLastHopeRange(const int botID) const;
    bool isBallWithMe() const;
    bool isBallAchievableInForwardMotion() const;
    void getFullAngularClearance(std::vector<std::pair<float, float> > &result, const int radius, const Vector2D<int> &center) const;
    bool getOppGoalClearance(const std::vector<std::pair<float, float> > &obstacles, const Vector2D<int> &center, std::pair<float, float> &result) const;
    bool getOurGoalClearance(const std::vector<std::pair<float, float> > &obstacles, const Vector2D<int> &center, std::pair<float, float> &result) const;
    bool getBotClearance(const std::vector<std::pair<float, float> > &obstacles, const Vector2D<int> &center, std::pair<float, float> &result, const Vector2D<int> &oppPos) const;
    // List the stateless decisive skill calling functions here.
    void kickBall(int speed);
    void kickBallFullSpeed() ;
    void kickBallForPass(const float distance);
    void preciseKick(float angle, int speed);
    void turnToTargetWithBall(const float goalAngle);
    void moveToBlockBall();
    void gotoStaticBall(const float finalSlope);
    void captureBall(const float targetAngle);
    bool canBlockBall(float &dist_from_line) const;
    void gotoPoint(int x, int y, bool align, float finalSlope, float finalVelocity);
    void gotoPointQuick(int x, int y, bool align, float finalSlope, float finalVelocity);
    void gotoPointExact(int x, int y, bool align, float finalSlope, float finalVelocity);
    void standAndDribble();
    void turnToAngle(const float finalSlope, const int maxOmega = MAX_BOT_OMEGA);
    void stopBot();
    void coverBall();
    int selectBestBotforPassReceive(const Vector2D<int> passer, const int preferred_bot, const int passerid) const;
    void kickForGoalOrPassBall();
  
  private:
    void addsegment(std::vector<std::pair<float, float> >&, const Vector2D<int>&, const Vector2D<int>&, const Vector2D<int>&) const;
  }; // class Tactic
} // namespace MyStrategy
#endif // TACTIC_H
