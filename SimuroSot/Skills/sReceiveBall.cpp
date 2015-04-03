#include "stdafx.h"
#include "skillSet.h"
#include "../HAL/comm.h"

namespace MyStrategy
{
  void SkillSet::receiveBall(const SParam &param)
  {
    // TODO Write this skill
    // after the trainers get working perfectly fine.
    /*
    static Vector2D<float> ballInitialpos;
    Vector2D<float> ballFinalpos;
    Vector2D<float> currentBotpos = state->homePos, botDestination;
    ballInitialpos = state->ballPos;
    ballFinalpos = state->ballPos;
    float avgSpeed = MAX_BOT_SPEED/2; // retrieve this from the speed trainer
    float a, b, c1, c2;
    a = ballFinalpos.x - ballInitialpos.x;
    b = ballFinalpos.y - ballFinalpos.y;
    c1 = currentBotpos.y * (-b) + currentBotpos.x * (-a);
    c2 = ballFinalpos.x * ballInitialpos.y - ballFinalpos.y * ballInitialpos.x;

    botDestination.x = -(a * c1 + b * c2) / (a * a + b * b);
    botDestination.y = (a * c2 - b * c1) / (a * a + b * b);
    param.BotConfig.x = botDestination.x;
    param.BotConfig.y = botDestination.y;
    param.BotConfig.finalSlope = 0;
    goToPoint(param);
    **/
  }
}
