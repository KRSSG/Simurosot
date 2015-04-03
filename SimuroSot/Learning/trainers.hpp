#ifndef TRAINERS_HPP
#define TRAINERS_HPP
#include "..\Utils\kalman.h"
#include "..\Core\beliefState.h"
 
extern void start_goToPointTrainer(MyStrategy::Kalman &, MyStrategy::BeliefState &, bool &run);
extern void start_TacticTester    (MyStrategy::Kalman &, MyStrategy::BeliefState &, bool &run);
extern void start_goToBallTrainer (MyStrategy::Kalman &, MyStrategy::BeliefState &, bool &run);
extern void train_scrap           (MyStrategy::Kalman &, MyStrategy::BeliefState &, bool &run);
#endif