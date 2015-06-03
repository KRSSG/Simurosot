	// MyStrategy.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Strategy.h"
#include "./Core/tester.hpp"
#include "./HAL/comm.h"
#include <math.h>
#include "winDebugger/Client.h"
static bool run = true;
const bool test =true;            // always true Dont TOUCH !! :p
bool FIELD_IS_INVERTED =false;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}



char myMessage[200]; //big enough???
//void Velocity ( Robot *robot, int vl, int vr );
MyStrategy::BeliefState state;
 Tester t(run, state);
   Kalman      kFilter;
   Util::CS writer_mutex;
Util::CS writer_preference;
 int counter;
  

extern "C" STRATEGY_API void Create ( Environment *env )
{
	Comm::getInstance()->envi=env;
	////Client::debugClient=Client::getInstance();	
	counter=0;
	char data[250];
//	  MyStrategy::Robot::writer_mutex      = &writer_mutex;
//  MyStrategy::Robot::writer_preference = &writer_preference;
	sprintf(data,"intit connection %d\n",counter);
	//Client::debugClient->SendMessages(data);
}

extern "C" STRATEGY_API void Destroy ( Environment *env )
{
	// free any user data created in Create ( Environment * )

	// eg. if ( env->userData != NULL ) delete ( MyVariables * ) env->userData;
}


extern "C" STRATEGY_API void Strategy ( Environment *env )
{
	//Client::debugClient->SendMessages("ok i am here\n");
	static int flag = 0;
//  if(flag%2==0)
 // {
  Comm::getInstance()->envi=env;
  kFilter.addInfo(env, FIELD_IS_INVERTED);
	kFilter.update(state);
  state.update();
  //}
	  t.run();
	int testInt = 100;	

	counter++;
	/*char data[250];
	wsprintf(data,"sending data: %d\n",counter);
	//Client::debugClient->SendMessages(data);
  */
}
