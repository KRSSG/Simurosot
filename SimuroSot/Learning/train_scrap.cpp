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
void train_scrap(Kalman &kFilter, BeliefState &state, bool &run)
{
  const int xmax = HALF_FIELD_MAXX*0.9;
  const int ymax = HALF_FIELD_MAXY*0.9;
  printf("Starting Scrap...\n");

  // Creating Trainer objs for pfac(Profile Factor) and ofac(Omega Factor)
  krr_trainer<kernel_type> trainer_pfac;
  krr_trainer<kernel_type> trainer_ofac;
  std::vector<sample_type> samples;
  std::vector<double>     result_pfac;
  std::vector<double>     result_ofac;
  sample_type g_fac;
  // Define the sampling rate in each  dimension x, y, angle
  int num_x = 5;
  int num_y = 5;
  int num_a = 5;
  /* Initialisation if no data is already present trained */
  for(int x1 = 0; x1 < num_x; x1++)
  {
    for(int y1 = 0; y1 < num_y; y1++)
    {
      for(int a1 = 0; a1 < num_a; a1++)
      {
        double del_x = -2 * xmax + ((4 * xmax) / (num_x - 1)) * x1;
        double del_y = -2 * ymax + ((4 * ymax) / (num_y - 1)) * y1;
        double del_a = -PI + ((2 * PI) / (num_a - 1)) * a1;
        g_fac(0) = del_x;
        g_fac(1) = del_y;
        g_fac(2) = del_a;
        samples.push_back(g_fac);
        result_pfac.push_back(1.0);
        result_ofac.push_back(1.0);
      }
    }
  }
  const double gamma = 3.0/compute_mean_squared_distance(randomly_subsample(samples, 100));
  cout << "using gamma of " << gamma << endl;
  trainer_pfac.set_kernel(kernel_type(gamma));
  trainer_ofac.set_kernel(kernel_type(gamma));
  decision_function<kernel_type> test_pfac = trainer_pfac.train(samples, result_pfac);
  decision_function<kernel_type> test_ofac = trainer_ofac.train(samples, result_ofac);
  g_fac(0) = 3.23; g_fac(1) = 412.23; g_fac(2) = 1043.0;
  printf("retrieved %lf %lf\n", test_pfac(g_fac), test_ofac(g_fac));
}
