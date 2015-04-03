#pragma once
#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "..\common\include\geometry.hpp"
#include "..\common\include\config.h"
namespace MyStrategy {
  class Evaluator {
    
  public:
    Evaluator(){}
    ~Evaluator(){}
    
    static inline int aimAtTarget(const Vector2D<int> &botPos, const float botAngle, const Vector2D<int> &aimPos) {
      return fabs((float)normalizeAngle(Vector2D<int>::angle(aimPos, botPos)-botAngle))*1000;
    }
    
    static inline int aimAtTarget(double angle) {
      return angle*1000;
    }
    
    static inline int turnToBall(const Vector2D<int> &botPos, const float botAngle, const Vector2D<int> &ballPos, const Vector2D<float> &ballVel) {
      if(ballVel.absSq() > ZERO_VELOCITY_THRESHOLD_SQ)
      {
        float ballAngle = Vector2D<float>::angle(ballVel, Vector2D<float>(0,0));
        return Evaluator::aimAtTarget(ballPos, ballAngle, botPos) + Evaluator::aimAtTarget(botPos, botAngle, ballPos);
      }
      return Evaluator::aimAtTarget(botPos, botAngle, ballPos);
    }
    
    static inline int goToPoint(const Vector2D<int> &botPos, const Vector2D<int> &finalPos) {
      return (botPos-finalPos).absSq();
    }
    
    static inline int goToPoint(const double dist) {
      return dist*dist;
    }
    
    static inline int goToPointWithAngle(const Vector2D<int> &botPos, const float botAngle, const Vector2D<int> &finalPos, const float finalSlope) {
      return fabs((float)normalizeAngle(finalSlope-botAngle))*1000 + (finalPos-botPos).absSq();
    }
    
    static inline void meetingPoint(Vector2D<int> &movingObjPos, Vector2D<float> &movingObjVel, Vector2D<int> &obj2pos, float &alpha, int &x, int&y)
    {
      x = ((movingObjVel.x * movingObjVel.x) * obj2pos.x + (movingObjVel.y * movingObjVel.y) * movingObjPos.x - movingObjVel.x * movingObjVel.y * (movingObjPos.y - obj2pos.y)) /
          ((movingObjVel.x * movingObjVel.x) + (movingObjVel.y * movingObjVel.y));
      y = ((movingObjVel.x * movingObjVel.x) * movingObjPos.y + (movingObjVel.y * movingObjVel.y) * obj2pos.y - movingObjVel.x * movingObjVel.y * (movingObjPos.x - obj2pos.x)) /
          ((movingObjVel.x * movingObjVel.x) + (movingObjVel.y * movingObjVel.y));
      alpha = (x - movingObjPos.x) / movingObjVel.x;
    }
  
  };
}

#endif // EVALUATOR_H
