#pragma once

#ifndef NAIVE_PS_H
#define NAIVE_PS_H

#include "play.hpp"
#include "ps.h"

namespace MyStrategy
{
  class BeliefState;

  class NaivePS : public PS
  {
  public:
    NaivePS(const BeliefState* state);
    ~NaivePS();

    void updateWeights(Play::Result termResult) const;
  }; // class NaivePS
} // namespace MyStrategy

#endif // NAIVE_PS_H
