#ifndef TDEFENDLINE_HPP
#define TDEFENDLINE_HPP

#include <list>
#include "../Tactics/tactic.h"
#include "../Skills/skillSet.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../Utils/intersection.hpp"
#include "comdef.h"

namespace MyStrategy
{
  class TDefendLine : public Tactic
  {
  public:
    TDefendLine(const BeliefState* state, int botID) :
    Tactic(Tactic::DefendLine, state, botID)
    {
      iState = POSITION;
    } // TDefendLine

    ~TDefendLine()
    { } // ~TDefendLine

    enum InternalState
    {
      POSITION,
      CLOSE_TO_BALL,
      SPINNING_CCW,
      SPINNING_CW
    } iState;

    inline bool isActiveTactic(void) const
    {
      return false;
    }


    void execute(const Param& tParam)
    {
      char debug[250];

      switch(iState)
      {
        case POSITION:
          {
            getbotdest(tParam,&sParam.GoToPointP.x,&sParam.GoToPointP.y,&sParam.GoToPointP.finalslope);
            sParam.GoToPointP.align = false;
            sID = SkillSet::GoToPoint;

            if( Vector2D<int>::dist(state->ballPos,state->homePos[botID]) < BOT_BALL_THRESH*1.1 )
              iState = CLOSE_TO_BALL;
            break;
          }

        case CLOSE_TO_BALL:
        {
          if(fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x))) < PI / 2 + PI / 10 
          && fabs((float)normalizeAngle(state->homeAngle[botID] - atan2(state->homePos[botID].y - state->ballPos.y, state->homePos[botID].x - state->ballPos.x)))  > PI / 2 - PI / 10
          && ForwardX(state->ballPos.x)>ForwardX(state->homePos[botID].x))
          {
            if(state->ballPos.y > 0)
              iState = FIELD_IS_INVERTED? SPINNING_CCW : SPINNING_CW;
            else
              iState = FIELD_IS_INVERTED? SPINNING_CW : SPINNING_CCW;
          }

          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.1)
          {
            iState = POSITION;
          }

          break;
        }

        case SPINNING_CW:
          {
            //sprintf(message,"SPINCW\n");
            ////Client::debugClient->SendMessages(message);
            sID = SkillSet::Spin;
            sParam.SpinP.radPerSec = (MAX_BOT_OMEGA);

            if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.1)
            {
              iState = POSITION;
            }
            
            break;    

          }

        case SPINNING_CCW:
        {
          //sprintf(message,"SPINCCW\n");
          ////Client::debugClient->SendMessages(message);
          sID = SkillSet::Spin;
          sParam.SpinP.radPerSec = -(MAX_BOT_OMEGA);

          if(Vector2D<int>::dist(state->ballPos,state->homePos[botID]) > BOT_BALL_THRESH*1.1)
          {
            iState = POSITION;
          }
           
          break;
        }

      }
      skillSet->executeSkill(sID, sParam);
    }

    void getbotdest(Param tParam,float *x,float *y,float *slope)
    {
      Vector2D<float>p1(tParam.DefendLineP.x1,tParam.DefendLineP.y1);
      Vector2D<float>p2(tParam.DefendLineP.x2,tParam.DefendLineP.y2);
      Vector2D<float>b(state->ballPos.x,state->ballPos.y);
      float m;
      
      //calculating x , y and slope required
      if(p2.x==p1.x) 
      {
        *x = p1.x;
        *y = state->ballPos.y;
        *slope = PI/2;
      }
      else if(p2.y==p1.y)
      {
        *x = state->ballPos.x;
        *y = p2.y;
        *slope = 0;
      }
      else
      { 
        m = (p2.y-p1.y)/(p2.x-p1.x);      
        *x = ( m*(b.y-p1.y) + m*m*p1.x + b.x)/(1 + m*m);
        *y = m*(*x - p1.x) + p1.y;
        *slope = atan(m);
      }
      
      Vector2D<float>p(*x,*y);

      //setting limits on x and y
      if((*x-p1.x)*(*x-p2.x)>0 || (*y-p1.y)*(*y-p2.y)>0)
      {
        if(abs(*x-p1.x)>abs(*x-p2.x) || abs(*y-p1.y)>abs(*y-p2.y))
          *x = p2.x,*y = p2.y;
        else *x = p1.x,*y = p1.y;
      }
    }
  }; // class TDefendLine
} // namespace MyStrategy

#endif // TDEFENDLINE_HPP
