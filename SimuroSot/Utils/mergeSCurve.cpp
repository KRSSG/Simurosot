#include "stdafx.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "comdef.h"
#include "pathPlanners.h"
#include <algorithm>
#include <fstream>

namespace MyStrategy
{
	static long int ticks = 0;
	static long int factor = 0;
	static int flag = 0;
  MergeSCurve::MergeSCurve() :
    Obstacles(NULL),
    nObstacles(0),
    nBotID(-1)
  { }

  MergeSCurve::~MergeSCurve()
  { }

  int MergeSCurve::Connect(Vector2D<int> Start,
                           Vector2D<int> End,
                           obstacle*       obs,
                           int             nObs,
                           int             BotID,
                           int*            nearest,
                           bool            TeamBlue)
  {
    float A, B, C, D, dot, len_sq, param, xx, yy, minParam = 999999;
    int minIndex = 0, i, count = 0;
    for(i = 0; i < nObs; ++i)
    {
      A = obs[i].x - Start.x; // vector from one point to test point
      B = obs[i].y - Start.y;
      C = End.x - Start.x; //vector from one endpoint to the other end point
      D = End.y - Start.y;

      dot = A * C + B * D;
      len_sq = C * C + D * D;
      param = dot / len_sq; // length range reduced to (0,1)

      if(param < 0)
      {
        xx = Start.x;
        yy = Start.y;
      }
      else if(param > 1)
      {
        xx = End.x;
        yy = End.y;
      }
      else
      {
        xx = Start.x + param * C;
        yy = Start.y + param * D;
      }

      if((obs[i].x - xx) * (obs[i].x - xx) + (obs[i].y - yy) * (obs[i].y - yy) < obs[i].radius * obs[i].radius)
      {
        count++;
        if(param < minParam)
        {
          minIndex = i;
          minParam = param;
        }
      }
    }

    if(nearest)
    {
      *nearest = minIndex;
      /*      if(count !=0)
            {
                for(i = 0;i<nObs;++i)
                {
                    len_sq = (obs[*nearest].x-obs[i].x)*(obs[*nearest].x-obs[i].x) + (obs[*nearest].y-obs[i].y)*(obs[*nearest].y-obs[i].y);
                    if(len_sq < (obs[i].radius*obs[i].radius))
                        count++;
                }
            }*/
    }
    return count;
  }

  int MergeSCurve::ifInObstacle(Vector2D<int> point, obstacle *obs, int nObs, int closest)
  {
    float dist_sq;
    for(int i = 0; i < nObs; ++i)
    {
      if(i != closest)
      {
        dist_sq = (point.x - obs[i].x) * (point.x - obs[i].x) + (point.y - obs[i].y) * (point.y - obs[i].y);
        if(dist_sq < (obs[i].radius * obs[i].radius))
          return i;
      }
    }
    return -1;
  }

  Vector2D<int> MergeSCurve::getNextPt(Vector2D<int> initial, int closest, Vector2D<int> final, obstacle *obstacles, int nObs, int BotID, bool teamBlue)
  {
    //// LoggertoStdOut("Getting point\n");
    float dist_sq = ((initial.x - obstacles[closest].x) * (initial.x - obstacles[closest].x)) + ((initial.y - obstacles[closest].y) * (initial.y - obstacles[closest].y));
    float rad_sq = obstacles[closest].radius * obstacles[closest].radius, x, xx, yy, dx, dy;
    Vector2D<int> clearerPoint, V1, V2;
    int cV1, cV2;

    xx = (obstacles[closest].x - initial.x) / sqrt(dist_sq);
    yy = (obstacles[closest].y - initial.y) / sqrt(dist_sq);

    dx = (final.x - initial.x) / sqrt(dist_sq);
    dy = (final.y - initial.y) / sqrt(dist_sq);

    if(dist_sq - rad_sq < 500)
    {
      V1.x = initial.x + yy * 500;
      V1.y = initial.y - xx * 500;
      V2.x = initial.x - yy * 500;
      V2.y = initial.y + xx * 500;

      if(yy * dx - xx * dy > 0)
        return V1;
      else
        return V2;
    }
    else
    {
      x = dist_sq * rad_sq / (dist_sq - rad_sq);
      x = sqrt(x) * 1.2f;

      V1.x = obstacles[closest].x + yy * x;
      V1.y = obstacles[closest].y - xx * x;
      V2.x = obstacles[closest].x - yy * x;
      V2.y = obstacles[closest].y + xx * x;

      cV1 = Connect(V1, final, obstacles, nObs, BotID, NULL, teamBlue);
      cV2 = Connect(V2, final, obstacles, nObs, BotID, NULL, teamBlue);
//          if(cV1 == cV2)
//          {
      if(yy * dx - xx * dy > 0)
        return V1;
      else
        return V2;
//          }
//          else if(cV1 > cV2)
//              return V2;
//          else
//              return V1;
    }
  }
  Vector2D<int> MergeSCurve::getNextPtRec(Vector2D<int> initial, int lastClosest, int closest, obstacle *obstacles, int nObs)
  {
    float dist_sq = ((initial.x - obstacles[closest].x) * (initial.x - obstacles[closest].x)) + ((initial.y - obstacles[closest].y) * (initial.y - obstacles[closest].y));
    float rad_sq = obstacles[closest].radius * obstacles[closest].radius;
    float x = dist_sq * rad_sq / (dist_sq - rad_sq);
    float xx = (obstacles[closest].x - initial.x) / sqrt(dist_sq);
    float yy = (obstacles[closest].y - initial.y) / sqrt(dist_sq);
    Vector2D<int> V1;
    Vector2D<int> V2;
    V1.x = obstacles[closest].x + yy * sqrt(x);
    V1.y = obstacles[closest].y - xx * sqrt(x);
    V2.x = obstacles[closest].x - yy * sqrt(x);
    V2.y = obstacles[closest].y + xx * sqrt(x);
    Vector2D<int> clearerPoint;
    float distSq1 = Vector2D<int>::distSq(V1, Vector2D<int>(obstacles[lastClosest].x, obstacles[lastClosest].y));
    float distSq2 = Vector2D<int>::distSq(V2, Vector2D<int>(obstacles[lastClosest].x, obstacles[lastClosest].y));
    if(distSq1 < distSq2)
      clearerPoint = V2;
    else
      clearerPoint = V1;

    int nearestObstacle = ifInObstacle(clearerPoint, obstacles, nObs, closest);

    if(nearestObstacle != -1)
      return getNextPtRec(initial, closest, nearestObstacle, obstacles, nObs);
    else
      return clearerPoint;
  }

  void MergeSCurve::ConnectObstacles(bool *Connected, int nCurrentObstacle)
  {
    int nCounterI;
    for(nCounterI = 0; nCounterI < nObstacles; ++nCounterI)
    {
      if(nCounterI == nBotID || Connected[nCounterI])
        continue;
      if( (Obstacles[nCounterI].x - Obstacles[nCurrentObstacle].x) *
          (Obstacles[nCounterI].x - Obstacles[nCurrentObstacle].x) +
          (Obstacles[nCounterI].y - Obstacles[nCurrentObstacle].y) *
          (Obstacles[nCounterI].y - Obstacles[nCurrentObstacle].y) <
          4 * Obstacles[nCounterI].radius * Obstacles[nCounterI].radius)
      {
        Connected[nCounterI] = true;
        ConnectObstacles(Connected, nCounterI);
      }
    }
  }

  int MergeSCurve::MergeObstacles()
  {
    bool bChecked[MAX_OBSTACLES], bConnected[MAX_OBSTACLES];
    obstacle Merged[MAX_OBSTACLES];
    int nMaxMerged = 0;
    float minx, miny, maxx, maxy, dRadius = Obstacles[0].radius;

    int nCounterI, nCounterJ, nMergedObstacles = nObstacles;

    for(nCounterI = 0; nCounterI < nObstacles; ++nCounterI)
    {
      bChecked[nCounterI] = false;
      bConnected[nCounterI] = false;
    }

    for(nCounterI = 0; nCounterI < nObstacles; ++nCounterI)
    {
      if(bChecked[nCounterI] || nBotID == nCounterI)
        continue;
      minx = Obstacles[nCounterI].x;
      miny = Obstacles[nCounterI].y;
      maxx = Obstacles[nCounterI].x;
      maxy = Obstacles[nCounterI].y;
      bConnected[nCounterI] = true;
      ConnectObstacles(bConnected, nCounterI);

      for(nCounterJ = 0; nCounterJ < nObstacles; ++nCounterJ)
      {
        if(bConnected[nCounterJ])
        {
          if(Obstacles[nCounterJ].x < minx)
            minx = Obstacles[nCounterJ].x ;
          if(Obstacles[nCounterJ].y < miny)
            miny = Obstacles[nCounterJ].y ;
          if(Obstacles[nCounterJ].x > maxx)
            maxx = Obstacles[nCounterJ].x ;
          if(Obstacles[nCounterJ].y > maxy)
            maxy = Obstacles[nCounterJ].y ;
          bConnected[nCounterJ] = false;
          bChecked[nCounterJ] = true;
        }
      }

      Merged[nMaxMerged].x = (maxx + minx) / 2;
      Merged[nMaxMerged].y = (maxy + miny) / 2;
      Merged[nMaxMerged].radius = sqrt((maxx - Merged[nMaxMerged].x) *
                                       (maxx - Merged[nMaxMerged].x) +
                                       (maxy - Merged[nMaxMerged].y) *
                                       (maxy - Merged[nMaxMerged].y))
                                  + dRadius;
      nMaxMerged++;
    }
    for(nCounterI = 0; nCounterI < nMaxMerged; ++nCounterI)
    {
      Obstacles[nCounterI] = Merged[nCounterI];
    }
    return nMaxMerged;
  }
  void MergeSCurve::PolarBased(Vector2D<int> s, Vector2D<int> e,double etheta, double stheta,  int &vl, int &vr, double prevSpeed)
{
	static const double d          = 6.5*(2*HALF_FIELD_MAXX/220); //distance between wheels in cm
    static const double ticksToCmS = 1.08; //still only approximate...
    static const double fieldXConvert = 29.75;
    static const double fieldYConvert = 27.33333;
    static const double xUncertainty = 0;//0.5; // Uncertainty is in %age of max value. eg. 1% where fabs(x) <= 1000 means fabs(error) <= 10
    static const double yUncertainty = 0;//0.5;
    static const double thetaUncertainty = 0;//3;
		  const int timeLCMs = 16;
const double timeLC = timeLCMs*0.001;
const int NUMTICKS = 300;
    // NOTE: its preferable to call x(), y(), and theta() of each object exactly once since they may return different
    // values on each call.
    Vector2D<int> initial(s.x-e.x, s.y-e.y);
    //double etheta = 0//e.theta();
    double theta = normalizeAngle(stheta - etheta);//s.theta
   // rotate initial by -e.theta degrees;
    double newx = initial.x * cos(-etheta) - initial.y * sin(-etheta);
    double newy = initial.x * sin(-etheta) + initial.y * cos(-etheta);
    initial = Vector2D<int>(newx, newy);
    double rho = sqrt(initial.x*initial.x + initial.y*initial.y);
    double gamma = normalizeAngle(atan2(initial.y, initial.x) - theta + PI);
    double delta = normalizeAngle(gamma + theta);
     double k1 = 1 , k2 =4 , k3 = 5;//inital values :: k1 = 0.05, k2 = 4, k3 = 20;
    double v = k1*rho*cos(gamma);
    double w;
    if(gamma == 0) {
        w = k2*gamma+k1*cos(gamma)*(gamma+k3*delta);
    } 
	else 
	{
        w = k2*gamma+k1*sin(gamma)*cos(gamma)/gamma*(gamma + k3*delta);
    }
	//v *= ticksToCmS;
    vl = v - d*w/2;
    vr = v + d*w/2;
    double timeMs = 0.07*rho + 12 * sqrt(rho); // empirical
    double speed = timeMs/timeLCMs<(prevSpeed/MAX_BOT_LINEAR_VEL_CHANGE)?prevSpeed-MAX_BOT_LINEAR_VEL_CHANGE:prevSpeed+MAX_BOT_LINEAR_VEL_CHANGE;
    if(speed > MAX_BOT_SPEED)
        speed = MAX_BOT_SPEED;
    else if (speed < 0)
        speed = 0;
    double max = abs(vl)>abs(vr)?abs(vl):abs(vr);
    if(max > 0) {
        vl = (vl)*speed/max;
        vr = (vr)*speed/max;
	int	dist = Vector2D<int>::dist(e, s);
	ticks++;
	
	//to write it in a file 
   /*	if(dist < 2*BOT_BALL_THRESH)
	{ 
		if (flag!=0)
			return ;
	   std::ofstream fin ;
		   fin.open("C:/Users/Abhinav/Desktop/abcd.txt",std::ios::app);
	   fin<<"k1 : "<<k1<<" k2 : "<<k2<<" k3 : "<<k3<<"     ticks  : "<<ticks<<" \n " ;
	   fin.close();
	   flag = 1;
	   k1 = factor*k1;
	   k2 = factor*k2;
	   k3  = factor*k3;
		   ticks = 0 ;
		    factor++;
	}
	*/
	}
}
  void MergeSCurve::PolarBidirectional(Vector2D<int> s,Vector2D<int> e, double etheta, double stheta,int &vl, int &vr, double prevSpeed)
  {   
       static bool wasInverted = false; // keeps track of whether in the previous call, the bot was inverted or not.
    PolarBased(s,e,etheta,stheta,vl,vr,prevSpeed);
    double v = (vl+vr)/2.0;
    wasInverted = false;
    if(v < 0 || (v == 0 && wasInverted) ) {
		etheta = normalizeAngle(etheta+PI);
       // (normalizeAngle(s.theta()+PI)); //
        PolarBased(s, e,etheta,stheta, vl, vr, prevSpeed);
        std::swap(vl, vr);
        vl = -vl;
        vr = -vr;
        wasInverted = true;
    }
  
  
  
  }
  bool MergeSCurve::plan(const Vector2D<int>& initial,
                         const Vector2D<int>& final,
                         Vector2D<int> *pt1,
                         Vector2D<int> *pt2,
                         obstacle *obstacles,
                         int obstacle_count,
                         int current_id,
                         bool teamBlue)
  {
    int closest;
    nObstacles = obstacle_count;
    Obstacles = obstacles;
    nBotID = current_id;
    nObstacles = MergeObstacles();
    Vector2D<float> TangentPt;
    *pt1 = final;

    if(!Connect(initial, final, obstacles, nObstacles, current_id, &closest, teamBlue))
    {
      return true;
    }

    *pt1 = getNextPt(initial, closest, final, obstacles, nObstacles, current_id, teamBlue);

    if(!Connect(*pt1, final, obstacles, obstacle_count, current_id, &closest, teamBlue))
    {
      *pt2 = final;
    }
    else
    {
      *pt2 = getNextPt(*pt1, closest, final, obstacles, obstacle_count, current_id, teamBlue);
    }
    return false;
  }
}
