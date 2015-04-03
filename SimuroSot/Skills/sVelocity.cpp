#include "stdafx.h"
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
namespace MyStrategy
{
  void SkillSet::velocity(const SParam& param)
  {
    errorLog(botID,(param.VelocityP.vl+param.VelocityP.vr)/2,(param.VelocityP.vl-param.VelocityP.vr)/2);
    comm->sendCommand(botID,param.VelocityP.vl,param.VelocityP.vr);
    
    //comm.sendCommand(botID, 0, param.VelocityP.v, param.VelocityP.v_t, 0, false);
  } // velocity
}
