#include "stdafx.h"
#include "..\mainheaders.hpp"
#include "../Skills/skillSet.h"
#include "../common/dlib/dlib/svm.h"
#include "trainers.hpp"
#include "../common/include/randGenerator.h"
#include "../common/include/config.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
using namespace MyStrategy;
using namespace dlib;
using namespace Util;

const float DRIBBLER_BALL_THRESH_ERR = 0.1;
const float BOT_BALL_THRESH_ERR     = 0.1;
const string collectionName = "Trainer.GotoBall";
const bool load_trainer_from_file = false;

typedef struct node_gotoball
{
  int dribble_ball_threshold;
  int bot_ball_threshold;
  double exp_time;
  node_gotoball(int dribble_ball_dist, int bot_ball_dist, double e_time): 
        dribble_ball_threshold(dribble_ball_dist), 
        bot_ball_threshold(bot_ball_dist),
        exp_time(e_time) {}
  node_gotoball() {}
} node_gotoball;
bool operator<(const node_gotoball &a, const node_gotoball &b)
{
  return a.exp_time > b.exp_time;
}

void start_goToBallTrainer(Kalman &kFilter, BeliefState &state, bool &run)
{
  printf("Starting Goto Ball Trainer...");
  // Initialising skill params and skill obj for training purpose
  SkillSet sk(&state, 0);
  SParam param;
//  param.GoToBallP.intercept = false; // TODO what is this for?
  node_gotoball start_node(DRIBBLER_BALL_THRESH, BOT_BALL_THRESH, 60);
  priority_queue< node_gotoball > mq;
  mq.push(start_node);
  node_gotoball bestnode = start_node;
  std::vector<node_gotoball> last10(10, start_node);
  int curpos = -1;
  while(run && !mq.empty())
  {
    curpos = (curpos+1)%10;
    kFilter.update(state);
    node_gotoball nd = mq.top();
    mq.pop();
    SkillSet::comm->addAdjustBot(0, 0, (int)-HALF_FIELD_MAXX/2, (int)0, 0.75, 0, 0, 0);
    SkillSet::comm->addAdjustBall(HALF_FIELD_MAXX/2, 0, 0, 0, 0);
    SkillSet::comm->sendCommand(0, 0, 0);
    //SkillSet::dribble_ball_threshold = nd.dribble_ball_threshold;
    //SkillSet::bot_ball_threshold     = nd.bot_ball_threshold;
    double maxt = 61;
    Util::Timer tm;
    tm.start();
    while(tm.split() < maxt * 1000 && (
          Evaluator::goToPoint(state.homePos[0], state.ballPos) > SkillSet::Bot_Point_dis_penalty_l2 ||
          Evaluator::aimAtTarget(state.homePos[0], state.homeAngle[0], state.ballPos) > SkillSet::angle_penalty_l2 ||
          state.homeVel[0].absSq() > 1000) )
    {
      kFilter.update(state);
      sk.executeSkill(SkillSet::GoToBall, param);
      Sleep(10000);
    }
    // TODO add goback and see if ball is with us test
    double timetaken = tm.stopms() / 1000.0;
    nd.exp_time = timetaken;
    last10[curpos] = nd;
    if(timetaken < bestnode.exp_time*1.2)
    {
      bestnode = nd;
      for(int i = 0; i < 5; i++)
      {
        node_gotoball random_walk_node          = nd;
        random_walk_node.dribble_ball_threshold = nd.dribble_ball_threshold*(RandGenerator::get_rand_float(1.0, DRIBBLER_BALL_THRESH_ERR));
        random_walk_node.bot_ball_threshold     = nd.bot_ball_threshold*(RandGenerator::get_rand_float(1.0, BOT_BALL_THRESH_ERR));
        random_walk_node.exp_time               = timetaken;
        mq.push(random_walk_node);
      }
    }
  }
  sort(last10.begin(), last10.end());
  DataBox::addData("Trainer.GotoBall", "dribble_ball_threshold", last10[9].dribble_ball_threshold);
  DataBox::addData("Trainer.GotoBall", "bot_ball_threshold",     last10[9].dribble_ball_threshold);
}
