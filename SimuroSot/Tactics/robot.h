#ifndef ROBOT_H
#define ROBOT_H

#include "../common/include/thread.h"
#include "tactic.h"

// Forward Declarations
namespace Util
{
  class CS;
}

namespace MyStrategy
{
  class BeliefState;
}

namespace MyStrategy
{
  class Robot 
  {
    friend class PExec;

  public:
    // Stores the current tactic and is controlled by the Play Layer
    Tactic::ID     tIDSh;
    // Stores the current tactic's parameters and is controlled by the Play Layer
    Tactic::Param  tParamSh;
    // Stores the current tactic's state and is controlled by the Play Layer
    Tactic::TState tStateSh;
    // List of tactics: A map of ID -> Tactic objects
    Tactic*        tacticList[Tactic::MAX_TACTICS];
    // Mutexes for reader-writer synchronization
    //static Util::CS*  reader_mutex;
   // static Util::CS*  writer_mutex;
 //   static Util::CS*  writer_preference;
    static int        reader_count;
    
  private:
    int            botID;

    // Critical Section object for synchronizing access to the above two fields
    Util::CS*      tacticCS;
    // Belief State object that the robot will use
    BeliefState*   state;
    // Shared Belief State object between it and the main thread
    BeliefState&   stateSh;
    // The function in which the Tactic loop runs forever
    void run();

  public:
    Robot(BeliefState& stateSh,
          Util::CS*    tacticCS,
          int          botID);

    ~Robot();
  }; // class Robot
} // namespace MyStrategy

#endif // ROBOT_H
