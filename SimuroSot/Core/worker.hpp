#include "../mainheaders.hpp"
#include "../common/include/thread.h"
#include "../winDebugger/Client.h"
using namespace MyStrategy;
#ifndef RUN_REFBOX
#define USE_FAKE_REFREE
#endif

class Worker:public Util::Thread 
{
public:
  bool &running;
  BeliefState &state;
  Util::CS*  writer_mutex;
  Util::CS*  writer_preference;
      Util::CS loggerCS;
   // Util::Logger logger("timelog.log",// Util::// LoggerWrite, loggerCS);
    Kalman      &kFilter;
   
	Worker(bool &running_, BeliefState &state_, Util::CS* writer_mutex_, Util::CS* writer_preference_,Kalman &kfilter_):
		running(running_),state(state_),writer_mutex(writer_mutex_),writer_preference(writer_preference_),kFilter(kfilter_) {}
  void fake_refree(HAL::RefBoxCmd  &refBoxCmdSh) 
  {
    static int countdown = 0;
    if(state.pr_gameRunning) 
	{
      const int ballx = state.ballPos.x;
      const int bally = state.ballPos.y;
     if(ballx < ForwardX(-HALF_FIELD_MAXX) && bally >= OUR_GOAL_MINY && bally <= OUR_GOAL_MAXY)
      {
        //refBoxCmdSh.cmdCounter++;
        //refBoxCmdSh.cmd = 'S';
        countdown = 1000;
        SkillSet::comm->addAdjustBall(0, 0, 0, 0, 0);
        //refBoxCmdSh.goalsYellow++;
      } else if(ballx > ForwardX(HALF_FIELD_MAXX) && bally >= OPP_GOAL_MINY && bally <= OPP_GOAL_MAXY)
      {
        //refBoxCmdSh.cmdCounter++;
        //refBoxCmdSh.cmd = 'S';
        countdown = 1000;
        SkillSet::comm->addAdjustBall(0, 0, 0, 0, 0);
        //refBoxCmdSh.goalsBlue++;
      }
    } 
	else if(countdown <= 0)
	{
        //refBoxCmdSh.cmdCounter++;
        //refBoxCmdSh.cmd = 's';
        countdown = 1000;
    } 
	else 
	{
      countdown--;
    }
  }
  void run()
  {
	//  //Client::debugClient->SendMessages("Reacced Here 1\n");
  static  PExec       pExec(&state);
//  //Client::debugClient->SendMessages("Reacced Here 2\n");
   // Util::// LoggertoStdOut("Waiting for signal from Referee Box and SSLVision Client\n");

    //while (running)
    {
      //writer_preference->enter();
      //writer_mutex->enter();
      //kFilter.update(state);
      //writer_mutex->leave();
      //writer_preference->leave();
	}
//	//Client::debugClient->SendMessages("Reacced Here 1\n");
	 {
		 state.update();
         if (pExec.playTerminated())
        {
		//	//Client::debugClient->SendMessages("Reacced Here 2\n");
        //  Util::Logger::toStdOut("*************Play terminated.Select new play\n*********************");
         pExec.evaluatePlay();
		//  //Client::debugClient->SendMessages("Reacced Here 3\n");
          pExec.selectPlay();
        } 
		
       pExec.executePlay();
		
      }
  }
};