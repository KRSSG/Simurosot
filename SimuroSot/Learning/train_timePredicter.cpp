#include "stdafx.h"
#include "..\mainheaders.hpp"
#include "..\Skills\skillSet.h"
#include "..\common\dlib\dlib\svm.h"
#include "trainers.hpp"
#include "..\common\include\randGenerator.h"
#include <iostream>
#include <vector>
#include <string>
using namespace std;
using namespace MyStrategy;
using namespace dlib;
using namespace Util;
typedef matrix<double, 3, 1> sample_type;
typedef radial_basis_kernel<sample_type> kernel_type;
const string collectionName = "Trainer.TimePredicter";
const bool load_trainer_from_file = false;

void start_timePredicterTrainer(Kalman &kFilter, BeliefState &state, bool &run)
{
  printf("Starting Time Predictor Trainer...");
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
  krls<kernel_type> g_test_time(kernel_type(0.1), 0.001);
  sample_type g_fac;
  // Define the sampling rate in each  dimension x, y, angle
  int num_x = 5;
  int num_y = 5;
  int num_a = 5;
  if(!load_trainer_from_file)
  {
    /* Initialisation if no data is already present trained */
    for(int x1 = 0; x1 < num_x; x1++)
    {
      for(int y1 = 0; y1 < num_y; y1++)
      {
        for(int a1 = 0; a1 < num_a; a1++)
        {
          double del_x = -2 * HALF_FIELD_MAXX + ((4 * HALF_FIELD_MAXX) / (num_x - 1)) * x1;
          double del_y = -2 * HALF_FIELD_MAXY + ((4 * HALF_FIELD_MAXY) / (num_y - 1)) * y1;
          double del_a = -PI + ((2 * PI) / (num_a - 1)) * a1;
          g_fac(0) = del_x;
          g_fac(1) = del_y;
          g_fac(2) = del_a;
          g_test_time.train(g_fac, sqrt(del_x * del_x + del_y * del_y) / 400);
        }
      }
    }
  }
  else
  {
    // Loading timepredicter object
    {
      krls<kernel_type> g_test_time_temp(kernel_type(0.1), 0.001);
      ifstream fin((DataBox::dataDir + DataBox::getString(collectionName, "serialized_timepredicterfile")).c_str(), ios::binary);
      deserialize(g_test_time_temp, fin);
      fin.close();
      int num_x = 5;
      int num_y = 5;
      int num_a = 5;
      for(int x1 = 0; x1 < num_x; x1++)
      {
        for(int y1 = 0; y1 < num_y; y1++)
        {
          for(int a1 = 0; a1 < num_a; a1++)
          {
            double del_x = -2 * HALF_FIELD_MAXX + ((4 * HALF_FIELD_MAXX) / (num_x - 1)) * x1;
            double del_y = -2 * HALF_FIELD_MAXY + ((4 * HALF_FIELD_MAXY) / (num_y - 1)) * y1;
            double del_a = -PI + ((2 * PI) / (num_a - 1)) * a1;
            g_fac(0) = del_x;
            g_fac(1) = del_y;
            g_fac(2) = del_a;
            g_test_time.train(g_fac, g_test_time_temp(g_fac));
          }
        }
      }
    }
  }
  sample_type fac;
  while(run)
  {
    kFilter.update(state);
    double del_x  = HALF_FIELD_MAXX * RandGenerator::get_rand_float(0, 1);
    double del_y  = HALF_FIELD_MAXY * RandGenerator::get_rand_float(0, 1);
    double del_a  = PI * RandGenerator::get_rand_float(0, 1);
    double from_x = (del_x < 0) ? (HALF_FIELD_MAXX) : (-HALF_FIELD_MAXX);
    double from_y = (del_y < 0) ? (HALF_FIELD_MAXY) : (-HALF_FIELD_MAXY);
    double from_a = 0;
    double   to_x = from_x + del_x;
    double   to_y = from_y + del_y;
    double   to_a = from_a + del_a;
    printf("Starting Training from %lf %lf %lf -> %lf %lf %lf\n", from_x, from_y, from_a, to_x, to_y, to_a);
    param.GoToPointP.x = to_x;
    param.GoToPointP.y = to_y;
    param.GoToPointP.finalslope = to_a;
    Vector2D<int> finalPos(to_x, to_y);
    SkillSet::comm->addAdjustBot(0, 0, (int)from_x, (int)from_y, 0, 0, 0, 0);
    SkillSet::comm->sendCommand(0, 0, 0);
    double maxt = 61;
    Util::Timer tm;
    tm.start();
    while(tm.split() < maxt * 1000 && Evaluator::goToPointWithAngle(state.homePos[0], state.homeAngle[0], finalPos, to_a) > 1000)
    {
      kFilter.update(state);
      sk.executeSkill(SkillSet::GoToPoint, param);
      Sleep(10000);
    }
    double timetaken = tm.stopms() / 1000.0;
    g_fac(0) = del_x;
    g_fac(1) = del_y;
    g_fac(2) = del_a;
    g_test_time.train(g_fac, timetaken);
  }
  // Serializing pfac object
  {
    ofstream fout((DataBox::dataDir + DataBox::getString(collectionName, "serialized_timepredicterfile")).c_str(), ios::binary);
    serialize(g_test_time, fout);
    fout.close();
  }
}
