#include "stdafx.h"
#include "..\mainheaders.hpp"
#include "..\Skills\skillSet.h"
#include "../common/dlib/dlib/svm.h"
#include "trainers.hpp"
#include "../common/include/randGenerator.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;
using namespace MyStrategy;
using namespace dlib;
using namespace Util;

typedef matrix<double, 3, 1> sample_type;
typedef radial_basis_kernel<sample_type> kernel_type;

const float MAX_ERR_PROFILE_FAC = 0.1;
const float MAX_ERR_OMEGA_FAC   = 0.1;
const string collectionName = "Trainer.GotoPoint";
const bool load_trainer_from_file = true;
const int num_x = 5;
const int num_y = 5;
const int num_a = 5;

typedef struct node_gotopoint
{
  std::vector<double> result_pfac;
  std::vector<double> result_ofac;
  double exp_time;
  node_gotopoint(std::vector<double> &r_pfac, std::vector<double> &r_ofac): result_pfac(r_pfac), result_ofac(r_ofac), exp_time(61) {}
} node_gotopoint;
bool operator<(const node_gotopoint &a, const node_gotopoint &b)
{
  return a.exp_time > b.exp_time;
}

void start_goToPointTrainer(Kalman &kFilter, BeliefState &state, bool &run)
{
  srand(time(0));
  int max_sample_count = num_x*num_y*num_a;
  const int xmax = HALF_FIELD_MAXX*0.9;
  const int ymax = HALF_FIELD_MAXY*0.9;
  printf("Starting Goto Point Trainer...");
  // Initialising skill params and skill obj for training purpose
  SkillSet sk(&state, 0);
  SParam param;
  param.GoToPointP.x = 0;
  param.GoToPointP.y = 0;
  param.GoToPointP.finalslope = 0;
  param.GoToPointP.align = 0;
  param.GoToPointP.finalVelocity = 0;
  param.GoToPointP.bot_point_dist_penalty = SkillSet::Bot_Point_dis_penalty_l1;
  param.GoToPointP.angle_diff_penalty     = SkillSet::angle_penalty_l1;
  // Creating Trainer objs for pfac(Profile Factor) and ofac(Omega Factor)
  krr_trainer<kernel_type> trainer_pfac;
  krr_trainer<kernel_type> trainer_ofac;
  sample_type sample;
  int sample_pos = 0;
  sample(0) = 0; sample(1) = 0; sample(2) = 0;
  std::vector<sample_type> samples(max_sample_count, sample);
  std::vector<double>     result_pfac(max_sample_count, 1.0);
  std::vector<double>     result_ofac(max_sample_count, 1.0);
  decision_function<kernel_type> func_pfac;
  decision_function<kernel_type> func_ofac;
  if(!load_trainer_from_file)
  {
    for(int x1 = 0; x1 < num_x; x1++)
    {
      for(int y1 = 0; y1 < num_y; y1++)
      {
        for(int a1 = 0; a1 < num_a; a1++)
        {
          double del_x = -2 * xmax + ((4 * xmax) / (num_x - 1)) * x1;
          double del_y = -2 * ymax + ((4 * ymax) / (num_y - 1)) * y1;
          double del_a = -PI + ((2 * PI) / (num_a - 1)) * a1;
          sample(0) = del_x;
          sample(1) = del_y;
          sample(2) = del_a;
          samples[sample_pos++] = sample;
        }
      }
    }
    sample_pos = 0;
  }else {
    // Loading pfac, ofac object
    {
      ifstream fin((DataBox::dataDir + DataBox::getString(collectionName, "profile_and_omega_factors")).c_str());
      double pval, oval;
      while(fin >> sample(0) >> sample(1) >> sample(2) >> pval >> oval) {
        samples[sample_pos] = sample;
        result_pfac[sample_pos] = pval;
        result_ofac[sample_pos] = oval;
        sample_pos++;
        cout<<"loaded data point sample: "<<sample_pos<<endl;
        sample_pos %= max_sample_count;
      }
      fin.close();
    }
  }
  const double gamma = 3.0/compute_mean_squared_distance(randomly_subsample(samples, max_sample_count));
  cout << "using gamma of " << gamma << endl;
  trainer_pfac.set_kernel(kernel_type(gamma));
  trainer_ofac.set_kernel(kernel_type(gamma));
  while(run)
  {
    node_gotopoint stnode(result_pfac, result_ofac);
    int rand_p    = std::rand()%max_sample_count;
    double del_x  = samples[rand_p](0);
    double del_y  = samples[rand_p](1);
    double del_a  = samples[rand_p](2);
    double from_x = (del_x < 0) ? (xmax) : (-xmax);
    double from_y = (del_y < 0) ? (ymax) : (-ymax);
    double from_a = 0;
    double   to_x = from_x + del_x;
    double   to_y = from_y + del_y;
    double   to_a = from_a + del_a;
    printf("Starting Training from %lf %lf %lf -> %lf %lf %lf\n", from_x, from_y, from_a, to_x, to_y, to_a);
    param.GoToPointP.x = to_x;
    param.GoToPointP.y = to_y;
    param.GoToPointP.finalslope = to_a;
    param.GoToPointP.align = true;
    param.GoToPointP.finalVelocity = 0;
    param.GoToPointP.bot_point_dist_penalty = SkillSet::Bot_Point_dis_penalty_l1;
    param.GoToPointP.angle_diff_penalty     = SkillSet::angle_penalty_l1;
    Vector2D<int> finalPos(to_x, to_y);
    priority_queue< node_gotopoint > mq;
    node_gotopoint bestnode = stnode;
    mq.push(stnode);
    int cnt = 20;
    while(cnt-- && !mq.empty())
    {
      SkillSet::comm->addAdjustBot(0, 0, (int)from_x, (int)from_y, 0, 0, 0, 0);
      SkillSet::comm->sendCommand(0, 0, 0);
      Sleep(100000);
      kFilter.update(state);
      node_gotopoint nd = mq.top();
      mq.pop();
      printf("start function generation\n");
      func_pfac = trainer_pfac.train(samples, nd.result_pfac);
      printf("Profile factor function created\n");
      func_ofac = trainer_ofac.train(samples, nd.result_ofac);
      printf("Omega factor function created\n");
      double maxt = 20;
      Util::Timer tm;
      tm.start();
      SkillSet::gotopoint_profilefac = func_pfac;
      SkillSet::gotopoint_omegafac   = func_ofac;
      while(tm.split() < maxt * 1000 && Evaluator::goToPointWithAngle(state.homePos[0], state.homeAngle[0], finalPos, to_a) > SkillSet::Bot_Point_dis_penalty_l1 + SkillSet::angle_penalty_l1)
      {
        kFilter.update(state);
        sk.executeSkill(SkillSet::GoToPoint, param);
		Sleep(10000);
      }
      double timetaken = tm.stopms() / 1000.0;
      if(timetaken < bestnode.exp_time)
      {
        bestnode = nd;
        for(int i = 0; i < 5; i++)
        {
          node_gotopoint random_walk_node(nd.result_pfac, nd.result_ofac);
          random_walk_node.result_pfac[rand_p] *= RandGenerator::get_rand_float(1.0, MAX_ERR_PROFILE_FAC);
          random_walk_node.result_ofac[rand_p] *= RandGenerator::get_rand_float(1.0, MAX_ERR_OMEGA_FAC);
          random_walk_node.exp_time = timetaken;
          mq.push(random_walk_node);
        }
      }
      cout << "Attempt " << 20 - cnt << " Time:" << timetaken << endl;
    }
    result_pfac = bestnode.result_pfac;
    result_ofac = bestnode.result_ofac;
    
    // Dumping results object
    {
      ofstream fout((DataBox::dataDir + DataBox::getString(collectionName, "profile_and_omega_factors")).c_str());
      double pval, oval;
      int iter = 0;
      while(iter < max_sample_count) {
        sample_type &sample = samples[iter];
        fout << sample(0) << " " << sample(1) << " " << sample(2) << " " << result_pfac[iter] << " " << result_ofac[iter] << endl;
        iter++;
      }
      fout.close();
    }
    // Serializing pfac object
    {
      ofstream fout((DataBox::dataDir + DataBox::getString("Trainer.GotoPoint", "serialized_pfacfile")).c_str(), ios::binary);
      serialize(func_pfac, fout);
      fout.close();
    }
    // Serializing ofac object
    {
      ofstream fout((DataBox::dataDir + DataBox::getString("Trainer.GotoPoint", "serialized_ofacfile")).c_str(), ios::binary);
      serialize(func_ofac, fout);
      fout.close();
    }
  }
}
