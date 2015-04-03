#ifndef PLAY_BOOK_H
#define PLAY_BOOK_H

// Forward Declarations
namespace MyStrategy
{
  class BeliefState;
  class Play;
}

namespace MyStrategy
{
  class PlayBook
  {
  public:
    enum PlayID
    {
      TestPlay, //0
      SetPosition,
      Stop,
      PositionOurStrike, //3
      PositionOppStrike,
      OurGoalKick,
      OppGoalKick,//6
      SuperOffense,
      Offense1,
      Offense2,//9
	  Offense3,
      SuperDefense,
      Defense1,
      Defense2,//13
      OurFreeKick,
      OppFreeKick,
	  //
	 // PositionOurKickoff, //sorry, i didnt see PositionOurStrike already did the same thing... so i removed it.
		//	PositionOppKickoff,
		//	Kickoff,
		//	PositionOurPenalty,
		//	TakeGoalKick,
		PositionOurFreeKick,
		PositionOppFreeKick,
	//	PositionOurFreeBall,
		//PositionOppFreeBall,
	//	PositionOppPenalty,
	//	PositionOurGoalKick,
	//	PositionOppGoalKick,
	  //
      None,
      MAX_PLAYS //17
    };

  protected:
    const BeliefState* state;
    Play*              playList[MAX_PLAYS];

  public:
    PlayBook(const BeliefState* state);
    
    ~PlayBook();

    void reload(void);
  }; // class PlayBook
} // namespace MyStrategy

#endif // PLAY_BOOK_H