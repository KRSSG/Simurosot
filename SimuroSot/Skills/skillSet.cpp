// This file contains all the skillSet functons
#include "StdAfx.h"
#include <cassert>
#include <string>
#include "skillSet.h"
#include "../Utils/pathPlanners.h"
#include "../Utils/comdef.h"
#include "../Core/beliefState.h"
#include "../common/include/config.h"
#include "../HAL/comm.h"
#include "../common/include/geometry.hpp"
#include "../winDebugger/Client.h"
#include "../pose.h"
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <fstream>
#include "../winDebugger/Client.h"
using namespace HAL;
using namespace std;
using namespace dlib;

const double Pose::d          = 6.5; //distance between wheels in cm
const double Pose::ticksToCmS = 1.107; //still only approximate...
const double Pose::fieldXConvert = 23.79;
const double Pose::fieldYConvert = 22.02;
// NOTE(arpit): Uncertainties should be non-zero when simulating. Currently 0 since bot data is fetched from vision.
const double Pose::xUncertainty = 0;//0.5; // Uncertainty is in %age of max value. eg. 1% where fabs(x) <= 1000 means fabs(error) <= 10
const double Pose::yUncertainty = 0;//0.5;
const double Pose::thetaUncertainty = 0;//3;


namespace MyStrategy
{
  const int timeLCMs = 16;
  const double timeLC = timeLCMs*0.001;
  const int NUMTICKS = 300;
  char debug[250];
Comm*  SkillSet::comm = Comm::getInstance();

string SkillSet::skillsCollection           = "Skills";
string SkillSet::goToPointCollection        = ".GotoPoint";
bool  SkillSet::loadParamsFromFile          = true;
bool  SkillSet::skillParamsLoaded           = false;
float SkillSet::Bot_Point_dis_penalty_l1    = BOT_POINT_THRESH;
float SkillSet::Bot_Point_dis_penalty_l2    = BOT_POINT_THRESH;
float SkillSet::angle_penalty_l1            = DRIBBLER_BALL_ANGLE_RANGE;
float SkillSet::angle_penalty_l2            = DRIBBLER_BALL_ANGLE_RANGE;
float SkillSet::GoToPoint_omegaFactor       = 1.0;
float SkillSet::GoToPoint_profileFactorMult = 1.0;
int   SkillSet::dribble_ball_threshold      = DRIBBLER_BALL_THRESH;
int   SkillSet::bot_ball_threshold          = BOT_BALL_THRESH;
decision_function<radial_basis_kernel<matrix<double, 3, 1> > > SkillSet::gotopoint_profilefac;
decision_function<radial_basis_kernel<matrix<double, 3, 1> > > SkillSet::gotopoint_omegafac;
SkillSet::SkillSet(const BeliefState* state, const int botID) :
  state(state),
  botID(botID),
  prevVel(0)
{
  // Resetting the skill list
  for (int sID = 0; sID < MAX_SKILLS; ++sID)
  {
    skillList[sID] = NULL;
  }
  // Mapping the skill IDs to their corresponding function pointers
  skillList[Spin]           = &SkillSet::spin;
  skillList[Stop]           = &SkillSet::stop;
  skillList[Velocity]       = &SkillSet::velocity;
  skillList[GoToBall]       = &SkillSet::goToBall;
  skillList[GoToPoint]      = &SkillSet::goToPoint;
  skillList[GoToPointDW]    = &SkillSet::goToPointDW;
  skillList[TurnToAngle] = &SkillSet::turnToAngle;
  skillList[TurnToPoint] = &SkillSet::turnToPoint;
  skillList[DefendPoint] = &SkillSet::defendPoint;
  skillList[GoalKeeping] = &SkillSet::goalKeeping;
  skillList[GoToPointStraight] = &SkillSet::goToPointStraight;
  skillList[GoToBallStraight] = &SkillSet::goToBallStraight;
  skillList[ChargeBall]		   = &SkillSet::chargeBall;
  // Initialization check
  for (int sID = 0; sID < MAX_SKILLS; ++sID)
  {
    assert(skillList[sID] != NULL); // Skill enum not mapped to the corresponding skill function
  }
  // Loading Parameters from DataBox
  //errt = new ERRT(0.2f, 0.6f);
#ifdef LOCAL_AVOID
  pathPlanner = new LocalAvoidance();
  pathPlannerSec = new MergeSCurve();
#else
  pathPlanner = new MergeSCurve();
#endif
} // SkillSet
SkillSet::~SkillSet()
{
  delete pathPlanner;
#ifdef LOCAL_AVOID
  //delete pathPlannerSec;
#endif
} // ~SkillSet
/*TODO: Adding definitions of arbit functions, needs to be corrected (see SkillSet.h) */
bool SkillSet::pointyInField(Vector2D<int> final)
{
  if(final.y < -HALF_FIELD_MAXY + BALL_AT_CORNER_THRESH || final.y > HALF_FIELD_MAXY - BALL_AT_CORNER_THRESH)
    return false;
  else return true;
}
//TODO: Try and replace all _ functions with correct implementations
void SkillSet::errorLog(int botid,float translation,float rotation)
{
  /***************by Prasann*********/
  float t=translation;
  float r=rotation;
  errordistance = (float) fabs((float)Vector2D<int>::dist(prevpoint,state->homePos[botid])-predicted_distance );
  errororientation = normalizeAngle((state->homeAngle[botid]-prevorientation) - predicted_rotation);
  printf("/*******error********/ \n distance = %f \n orientation = %f \n/******error*******/\n",errordistance,errororientation);
  prevpoint = state->homePos[botid];
  prevorientation = state->homeAngle[botid];
  //FILE *f = fopen("/home/robo/botplot/compare_dataset/botlognew.txt", "a");
  //FILE *f1 = fopen("/home/robo/botplot/compare_dataset/botlogpoint.txt", "a");
  //fprintf(f, "%d %d %d %d %f %f %f\n", (int)(t-r), (int)(t+r), (int)state->homePos[botID].x, (int)state->homePos[botID].y, state->homeAngle[botID],fabs((float)errordistance),errororientation);
  //fprintf(f1,"%f\t%f\n",errordistance,predicted_distance );
  //fclose(f);
  //fclose(f1);
  predicted_distance = t;
  predicted_rotation = r;
  /***************by Prasann*********/
}

void SkillSet::_goToPoint(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance = CLEARANCE_PATH_PLANNER,bool increaseSpeed,bool trapz)
  {
    std::vector<obstacle> obs;
    obstacle obsTemp;
    for (int i = 0; i < HomeTeam::SIZE ; ++i)
    {
      /// Adding Condition to REMOVE all obstacles that are sufficiently CLOSE to BALL
      if( i != botid && Vector2D<int>::distSq(state->homePos[botid], state->homePos[i]) < COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->homePos[i].x;
          obsTemp.y = state->homePos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }

    for (int i = 0; i < AwayTeam::SIZE; ++i)
    {
      if(Vector2D<int>::distSq(state->homePos[botid], state->awayPos[i]) < COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->awayPos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->awayPos[i].x;
          obsTemp.y = state->awayPos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }
    Vector2D<int> point, nextWP, nextNWP;
    float r = 0, t = 0, dist = 0;
    dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  // Distance of next waypoint from the bot
    if(dist < BOT_POINT_THRESH )
    {  
       
      float angle = fabs((float)firaNormalizeAngle(state->homeAngle[botID] - finalslope));
      if(angle > DRIBBLER_BALL_ANGLE_RANGE)
      {
        SParam param;
        param.TurnToAngleP.finalslope = finalslope;
        turnToAngle(param);
        return;
      }
      comm->sendCommand(botID, finalvel, finalvel);
      return;
      // This part of the function is just for safety.
      // The tactic should actually prevent this call.
      // The bot should be aligned properly before this condition is reached.
    }

#ifdef LOCAL_AVOID
    pathPlanner->plan(state->homePos[botID],
                      state->homeVel[botID],
                      dpoint,
                      obs,
                      botID,
                      true,
                      state->homeAngle[botID],
                      finalslope,
                      t,
                      r,
                      comm,
                      clearance);
	
#endif

/*
	double prevVel=sqrt(pow(state->homeVel[botID].x,2)+pow(state->homeVel[botID].y,2));
	
     Vector2D<int> s  = state->homePos[botID];
	 Vector2D<int> e = dpoint;
	// double etheta = finalslope;
	// double stheta = state->homeAngle[botID];
	 int vl , vr;
	 double prevSpeed = prevVel;


	
    // NOTE: its preferable to call x(), y(), and theta() of each object exactly once since they may return different
    // values on each call.
	 //***************************
	  Vector2D<int> initial((x(s.x/fieldXConvert)- x(e.x/fieldXConvert)), (y(s.y/fieldYConvert)-y(e.y/fieldYConvert))); 
    double etheta = theta(finalslope);
	double stheta = theta(state->homeAngle[botID]);
    double theta = normalizeAngle(stheta - etheta) ;
	 //***************************
   
	double newx = initial.x * cos(-etheta) - initial.y * sin(-etheta);
    double newy = initial.x * sin(-etheta) + initial.y * cos(-etheta);
    initial = Vector2D<int>(newx, newy);
    double rho = sqrt(initial.x*initial.x + initial.y*initial.y);
    double gamma = normalizeAngle(atan2(initial.y, initial.x) - theta + PI);
    double delta = normalizeAngle(gamma + theta);
    double k1 = 0.20 , k2 =4 , k3 = 5;//inital values :: k1 = 0.05, k2 = 4, k3 = 20;
	//better versions
	// 0.2 , 4 , 20
	//0.2 , 4 , 5   this was better
     double v = k1*rho*cos(gamma);
    double w;
    if(gamma == 0) {
        w = k2*gamma+k1*cos(gamma)*(gamma+k3*delta);
    } else {
        w = k2*gamma+k1*sin(gamma)*cos(gamma)/gamma*(gamma + k3*delta);
    }
	v *= ticksToCmS;
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
        vl = (vl)*speed/max; //*1
        vr = (vr)*speed/max; //*1
	}
	*/
	
    float fTheta = asin(sqrt(fabs((float)r)));
    fTheta = 1 - fTheta/(PI/2);
    fTheta = pow(fTheta,2.2) ;
    float fDistance = (dist > BOT_POINT_THRESH*2) ? 1 : dist / ((float) BOT_POINT_THRESH *2);
    float fTot = fDistance;
    fTot = 0.2 + fTot*(1-0.2);

	float profileFactor = MAX_BOT_SPEED;
	if(trapz) profileFactor *= fDistance;
	
	//if(trapz) //Client::debugClient->SendMessages("TTTTTTTTPPPPPPPRRRRRRRRAAAAAAAAZZZZZZZZZZ\n");

    //if(increaseSpeed==1&&r<0.11)
//    if(fabs((float)r)<0.11)
//      profileFactor*=2;
////    //to limit change
//    {
//      if(fabs((float)profileFactor-prevVel)>MAX_BOT_LINEAR_VEL_CHANGE)
//      {
//        if(profileFactor>prevVel)profileFactor=prevVel+MAX_BOT_LINEAR_VEL_CHANGE;
//        else profileFactor=prevVel-MAX_BOT_LINEAR_VEL_CHANGE;
//      }
//      prevVel=profileFactor;
//    }
//    if(profileFactor>1.5*MAX_BOT_SPEED)
//      profileFactor = 1.5*MAX_BOT_SPEED;
//    else if(profileFactor <-1.5*MAX_BOT_SPEE D)
//      profileFactor = -1.5*MAX_BOT_SPEED;*
//    prevVel=profileFactor;
    r *= 0.25*profileFactor;
    t *= profileFactor;
	//Vector2D<int> velocity;
	//int vl=0 , vr=0;
//	double prevVel=sqrt(pow(state->homeVel[botID].x,2)+pow(state->homeVel[botID].y,2));
	//pathPlannerSec->PolarBased(state->homePos[botID], dpoint,finalslope,state->homeAngle[botID],finalvel,finalvel,prevVel);
	/*
	
	int vl , vr;
	static double prevVel=0;
	pathPlannerSec->PolarBidirectional(state->homePos[botID], dpoint,finalslope,state->homeAngle[botID],vl,vr,prevVel);
	prevVel = max(fabs((float)vl),fabs((float)vr));

	#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID,vl,vr);
#else
    comm->sendCommand(botID,vl,vr);
#endif
	*/
	
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, (t - r), (t + r));
#else
    comm->sendCommand(botID, (t - r), (t + r));
#endif

}
void SkillSet::_goToPointLessThresh(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance = CLEARANCE_PATH_PLANNER,bool increaseSpeed)
{
    std::vector<obstacle> obs;
    obstacle obsTemp;
    for (int i = 0; i < HomeTeam::SIZE ; ++i)
    {
      /// Adding Condition to REMOVE all obstacles that are sufficiently CLOSE to BALL
      if( i != botid && Vector2D<int>::distSq(state->homePos[botid], state->homePos[i]) < 0.25*COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->homePos[i].x;
          obsTemp.y = state->homePos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }

    for (int i = 0; i < AwayTeam::SIZE; ++i)
    {
      if(Vector2D<int>::distSq(state->homePos[botid], state->awayPos[i]) < 0.25*COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->awayPos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->awayPos[i].x;
          obsTemp.y = state->awayPos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }
    Vector2D<int> point, nextWP, nextNWP;
    float r = 0, t = 0, dist = 0;
    dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  // Distance of next waypoint from the bot
    
    
    if(dist < BOT_POINT_THRESH /2 )
    {
      float angle = fabs(firaNormalizeAngle(state->homeAngle[botID] - finalslope));
      if(angle > DRIBBLER_BALL_ANGLE_RANGE)
      {
        SParam param;
        param.TurnToAngleP.finalslope = finalslope;
        turnToAngle(param);
        return;
      }
     
      comm->sendCommand(botID, finalvel, finalvel);
      return;
      // This part of the function is just for safety.
      // The tactic should actually prevent this call.
      // The bot should be aligned properly before this condition is reached.
    }

#ifdef LOCAL_AVOID
    pathPlanner->plan(state->homePos[botID],
                      state->homeVel[botID],
                      dpoint,
                      obs,
                      botID,
                      true,
                      state->homeAngle[botID],
                      finalslope,
                      t,
                      r,
                      comm,
                      clearance);
#else
#error LOCAL_AVOIDANCE should always be defined!
#endif
    float fTheta = asin(sqrt(fabs(r)));
    fTheta = 1 - fTheta/(PI/2);
    fTheta = pow(fTheta,2.2) ;
    float fDistance = (dist > BOT_POINT_THRESH*3) ? 1 : dist / ((float) BOT_POINT_THRESH *3);
    float fTot = fDistance * fTheta;
    fTot = 0.2 + fTot*(1-0.2);
    float profileFactor = MAX_BOT_SPEED * fTot;
    //if(increaseSpeed==1&&r<0.11)
    if(fabs(r)<0.11)
      profileFactor*=2;
//    //to limit change
    {
      if(fabs(profileFactor-prevVel)>MAX_BOT_LINEAR_VEL_CHANGE)
      {
        if(profileFactor>prevVel)profileFactor=prevVel+MAX_BOT_LINEAR_VEL_CHANGE;
        else profileFactor=prevVel-MAX_BOT_LINEAR_VEL_CHANGE;
      }
      prevVel=profileFactor;
    }
    if(profileFactor>1.5*MAX_BOT_SPEED)
      profileFactor = 1.5*MAX_BOT_SPEED;
    else if(profileFactor <-1.5*MAX_BOT_SPEED)
      profileFactor = -1.5*MAX_BOT_SPEED;
    prevVel=profileFactor;
    r *= 0.1*profileFactor;
    t *= profileFactor;
	float max = fabs(t+r)>fabs(t-r)?fabs(t+r):fabs(t-r);
	float scale =120/max; 
	scale = 0.2*scale + 0.8*dist*scale/3000;
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, (t - r), (t + r));
#else
    comm->sendCommand(botID, (t - r)*scale, (t + r)*scale);
#endif
}

void SkillSet::_dribbleToPoint(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance = 80.0)
{
  float theta   = normalizeAngle(Vector2D<int>::angle(dpoint, state->homePos[botID]));
  float phiStar = finalslope;
  float phi     = (state->homeAngle[botID] < -PI) ? PI : state->homeAngle[botID];
  float dist    = Vector2D<int>::dist(dpoint, state->homePos[botID]);  // Distance of next waypoint from the bot
  float alpha   = normalizeAngle(theta - phiStar);
  float beta;
  if(fabs((float)alpha)  < fabs((float)atan2(clearance, dist)))
    printf("ALPHA!\n");
  else
    printf("c/d\n");
  beta = (fabs((float)alpha) < fabs((float)atan2(clearance, dist))) ? (alpha) : SGN(alpha) * atan2(clearance, dist);
  float thetaD  = normalizeAngle(theta + beta);
  float delta   = normalizeAngle(thetaD - phi);
  float r       = (sin(delta) * sin(delta) * SGN(sin(delta)));
  float t       = (cos(delta) * cos(delta)); //SGN(tan(delta));
  float ballBotAngle = fabs((float)normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[botID]) - state->homeAngle[botID]));
  if(ballBotAngle > PI / 2)
  {
    t = -t;
    r = -r;
  }
  printf("delta  = %f\n", ballBotAngle * 180 / PI);
  float profileFactor;
  profileFactor = MAX_BOT_SPEED * ((dist > MIN_DIST_FROM_TARGET) ? (1) : (dist / MIN_DIST_FROM_TARGET));
  printf("dist= %f\n", dist);
  if(dist < BOT_POINT_THRESH)
  {
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, profileFactor * (t - r), profileFactor * (t + r));
#else
    comm->sendCommand(botID, profileFactor * (t - r), profileFactor * (t + r));
#endif
  }
  else
  {
    comm->sendCommand(botID, 0, 0);
  }
}
void SkillSet::_goToPointStraight(int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance)
{
  float t;
  float phiStar, theta = normalizeAngle(Vector2D<int>::angle(dpoint, state->homePos[botID]));
  phiStar = finalslope;
  float phi = state->homeAngle[botID];
  float dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  // Distance of next waypoint from the bot
  //  float alpha = firaNormalizeAngle(theta - phiStar);
//    float beta;
//    beta = (fabs((float)alpha) < fabs((float)atan2(clearance, dist))) ? (alpha) : SGN(alpha) * atan2(clearance, dist);
  //  float thetaD = normalizeAngle(theta + beta);
  float delta = normalizeAngle(theta - phi);
  float ndelta = firaNormalizeAngle(delta);
  float r = 0.7*(sin(ndelta) * sin(ndelta)) * SGN(tan(ndelta));
  t = (cos(ndelta) * cos(ndelta)) * SGN(cos(delta));
  float profileFactor, velMod;
  velMod = ((state->homeVel[botID].x) * (state->homeVel[botID].x)) + ((state->homeVel[botID].y) * (state->homeVel[botID].y));
  velMod = sqrtf(velMod);
  //profileFactor =  ((dist>MIN_DIST_FROM_TARGET)?(MAX_BOT_SPEED):((MAX_BOT_SPEED-finalvel)*(dist/MIN_DIST_FROM_TARGET)+finalvel));
  //profileFactor = fabs((float)t) < 0.5 ? MAX_BOT_SPEED * 0.5 : MAX_BOT_SPEED * fabs((float)t) ;
  profileFactor=(MAX_BOT_SPEED/4.0 + 3/4.0* MAX_BOT_SPEED*(fabs((float)cos(ndelta))));
 // Util::// LoggertoStdOut("t: %f,r: %f",t,r);
  if(dist < BOT_POINT_THRESH )
  {
    SParam param;
    param.TurnToAngleP.finalslope = finalslope;
    stop(param);
 
  }
  else
  {
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, profileFactor * (t - r), profileFactor * (t + r));
#else
    if(fabs(delta)>PI/10)
    {
      char x[100];
      sprintf(x,"Here : %.5g \n\n ",delta);
   //   wsprintf(x,"Here \n\n %.5g \n ",delta);
      	//Client::debugClient->SendMessages(x);
      SParam p1;
      p1.TurnToAngleP.finalslope = theta;
      turnToAngle(p1);
    }
    else
      comm->sendCommand(botID, MAX_BOT_SPEED,MAX_BOT_SPEED);
    
#endif
  }
}
void SkillSet::_turnToAngle(float angle, float *vl, float *vr)
{
  //Util::// LoggertoStdOut("Bot turning by angle : %f\n",angle);
  float omega = (angle * 2 / PI) * MAX_BOT_OMEGA;
  if(angle < 0)
  {
    //*vl = omega < MIN_BOT_OMEGA ? MIN_BOT_OMEGA*BOT_RADIUS : omega*BOT_RADIUS;
    *vl = 5;
    *vr = -(*vl);
  }
  else
  {
    //*vl = omega < MIN_BOT_OMEGA ? -MIN_BOT_OMEGA*BOT_RADIUS : -omega*BOT_RADIUS;
    *vl = 5; 
    *vr = -(*vl);
  }
}
void SkillSet::_goToPointDW(int botid, Vector2D<int> dpoint, float finalvel, float finalslope)
{
static float vl =0 , vr =0 ;
	Pose s , e ;
	s = Pose(state->homePos[botid].x, state->homePos[botid].y, state->homeAngle[botid]);
    e = Pose(dpoint.x, dpoint.y, finalslope);
    const int del_v_max = 15; //ticks
    const float step = 1; //ticks
    const float max_vel = 1000; //120ticks
    const float a_r_max = 780; //380cm/s^2
//    const float PI= 3.14159;
    const float t=0.016;
    const float k=2;
    int count=0;
    double arr[10000][3];
	// sprintf(debug,"entering ");
    // Client::debugClient->SendMessages(debug);
	//sprintf(debug," homeVel.x :: %f || homeVel.y :: %f \n ",state->homeVel[botid].x,state->homeVel[botid].y) ; 
	float prevSpeed = (vl + vr)/2 ;//sqrt(pow(state->homeVel[botid].x, 2) + pow(state->homeVel[botid].y, 2));
	float prevOmega = 0.0 ; // (vl - vr)/(2*Pose::d);//(vr - vl)/(Pose::d) ;//state->homeOmega[botid] ; 
	//double prevSpeed = (state->homeVel[botid].x+state->homeVel[botid].y)/2;
   // double prevOmega = (prevVr- prevVl)/(Constants::d);
	//float prevSpeed, prevOmega,alpha,acc,theta,x,y,reqtheta,dtheta;

    //int i,j;                                                           //i=vr,,j=vl
	// sprintf(debug,"prevSpeed :: %f || prevomega :: %f \n ",prevSpeed,prevOmega) ;
    // Client::debugClient->SendMessages(debug);
    for(float del_vr=-del_v_max;del_vr<=del_v_max;del_vr+=step)
        {
            for(float del_vl=-del_v_max;del_vl<=del_v_max;del_vl+=step)
            {

                float newSpeed= prevSpeed + Pose::ticksToCmS*(del_vr+del_vl)/2; // cm/s
                float newOmega= prevOmega + Pose::ticksToCmS*(del_vr-del_vl)/Pose::d;  // cm/s                //D Is the constant breadth of the bot
//                qDebug()<<"Trying newSpeed = "<<newSpeed<<" newOmega = "<<newOmega;
				//sprintf(debug,"newSpeed :: %f || newomega :: %f \n ",newSpeed,newOmega) ;
				//Client::debugClient->SendMessages(debug);
				if(abs(newSpeed/Pose::ticksToCmS) >max_vel || abs(newOmega/Pose::ticksToCmS) >(2*max_vel)/Pose::d)
                    continue;
                if(abs((newSpeed+(Pose::d*newOmega)/2)) > max_vel ||  abs((newSpeed-(Pose::d *newOmega)/2)) > max_vel)
                    continue;
                if((newSpeed*newOmega)>=a_r_max*sqrt(1-pow((del_vr + del_vl)/(2*del_v_max),2)))
                    continue;                   // constraint from the equation of ellipse.
                //if(count>400) break;                                 //we would take only a fixed number of points under consideration
//                qDebug()<<"Trying newSpeed = "<<newSpeed<<" newOmega = "<<newOmega;
                // float alpha=(newOmega-prevOmega)/t; // rad/cm^2
                float theta= s.theta() + (newOmega*t); //rad
                theta = normalizeAngle(theta);

                 float x= s.x() + (newSpeed*cos(theta)*t);
                 float y= s.y() + (newSpeed*sin(theta)*t);

                float reqtheta=atan2((e.y()-y),(e.x()-x));

                float dtheta=firaNormalizeAngle(theta-reqtheta);

                arr[count][0]= sqrt(pow((x - e.x()),2) + pow((y - e.y()),2)) + k*pow((dtheta),2) ;         // our objective function
                arr[count][1]= newSpeed;
                arr[count][2]= newOmega;
				//sprintf(debug," count :: %lf || function :: %lf || newspeed :: %lf || newomega :: %lf ",count,arr[count][0],arr[count][1],arr[count][2]);
                //Client::debugClient->SendMessages(debug);
				count++;
            }
        }
    float min= arr[0][0];
    float best_v=arr[0][1];
    float best_w=arr[0][2];
	//sprintf(debug," best_v :: %lf || best_w :: %lf ",arr[0][1],arr[0][2]);
	for(int i=0;i<count;i++)
    {
        if(arr[i][0]<min)
        {
            min=arr[i][0];
            best_v= arr[i][1];
            best_w = arr[i][2];
        }
    }

    float theta= s.theta() + (best_w*t); //rad
    theta = normalizeAngle(theta);
    float x= s.x() + (best_v*cos(theta)*t);
    float y= s.y() + (best_v*sin(theta)*t);

    //outStream<<best_v<< '\t'<< best_w <<'\t'<< x <<'\t'<< y<<endl;
    vr=(best_v)+(Pose::d *best_w)/2 ;                    // update velocity

    vl=(2*best_v) - vr;                       // update omega
    vr/=Pose::ticksToCmS;
    vl/=Pose::ticksToCmS;
	//sprintf(debug," vl :: %d || vr :: %d ",vl,vr);
 
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, vl, vr);
#else
    comm->sendCommand(botID, vl, vr);
#endif
    return; 
}
void SkillSet::_goToPointPolar (int botid, Vector2D<int> dpoint, float finalvel, float finalslope, float clearance = CLEARANCE_PATH_PLANNER,bool increaseSpeed,bool trapz) {
    std::vector<obstacle> obs;
    obstacle obsTemp;
    for (int i = 0; i < HomeTeam::SIZE ; ++i)
    {
      /// Adding Condition to REMOVE all obstacles that are sufficiently CLOSE to BALL
      if( i != botid && Vector2D<int>::distSq(state->homePos[botid], state->homePos[i]) < COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->homePos[i].x;
          obsTemp.y = state->homePos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }

    for (int i = 0; i < AwayTeam::SIZE; ++i)
    {
      if(Vector2D<int>::distSq(state->homePos[botid], state->awayPos[i]) < COLLISION_DIST * COLLISION_DIST && Vector2D<int>::distSq(state->ballPos, state->awayPos[i]) > BOT_RADIUS * BOT_RADIUS * 9)
      {
        /// If destination is close to bot, don't add it as obstacle
        if(Vector2D<int>::distSq(dpoint, state->homePos[i]) > BOT_RADIUS * BOT_RADIUS * 1)
        {
          obsTemp.x = state->awayPos[i].x;
          obsTemp.y = state->awayPos[i].y;
          obsTemp.radius =  BOT_RADIUS;
          obs.push_back(obsTemp);
        }
      }
    }
    Vector2D<int> point, nextWP, nextNWP;
    float r = 0, t = 0, dist = 0;
    dist = Vector2D<int>::dist(dpoint, state->homePos[botID]);  // Distance of next waypoint from the bot
    if(dist < BOT_POINT_THRESH )
    {  
       
      float angle = fabs((float)firaNormalizeAngle(state->homeAngle[botID] - finalslope));
      if(angle > DRIBBLER_BALL_ANGLE_RANGE)
      {
        SParam param;
        param.TurnToAngleP.finalslope = finalslope;
        turnToAngle(param);
        return;
      }
      comm->sendCommand(botID, finalvel, finalvel);
      return;
      // This part of the function is just for safety.
      // The tactic should actually prevent this call.
      // The bot should be aligned properly before this condition is reached.
    }

#ifdef LOCAL_AVOID
    pathPlanner->plan(state->homePos[botID],
                      state->homeVel[botID],
                      dpoint,
                      obs,
                      botID,
                      true,
                      state->homeAngle[botID],
                      finalslope,
                      t,
                      r,
                      comm,
                      clearance);
	
#endif

    Pose s, e;
    s = Pose(state->homePos[botID].x, state->homePos[botID].y, state->homeAngle[botID]);
    e = Pose(dpoint.x, dpoint.y, finalslope);

    double prevSpeed, finalSpeed;
    prevSpeed = sqrt(pow(state->homeVel[botID].x, 2) + pow(state->homeVel[botID].y, 2));
    finalSpeed = finalvel;

    int vl, vr;
    

    // NOTE: its preferable to call x(), y(), and theta() of each object exactly once since they may return different
    // values on each call (when simulating, gaussian errors non-zero).
    Vector2D<int> initial(s.x()-e.x(), s.y()-e.y());
    double etheta = e.theta();
    double theta = normalizeAngle(s.theta() - etheta);
    // rotate initial by -e.theta degrees;
    double newx = initial.x * cos(-etheta) - initial.y * sin(-etheta);
    double newy = initial.x * sin(-etheta) + initial.y * cos(-etheta);
    initial = Vector2D<int>(newx, newy);
    double rho = sqrt(initial.x*initial.x + initial.y*initial.y);
    double gamma = normalizeAngle(atan2(initial.y, initial.x) - theta + PI);
    double delta = normalizeAngle(gamma + theta);
    double k1 = 0.05, k2 = 4 , k3 = 20; //k1 = 0.05, k2 = 4, k3 = 20;
    double v = k1*rho*cos(gamma);
    double w;
    if(gamma == 0) {
        w = k2*gamma+k1*cos(gamma)*(gamma+k3*delta);
    } else {
        w = k2*gamma+k1*sin(gamma)*cos(gamma)/gamma*(gamma + k3*delta);
    }
    // velocity profiling based on curvature
    double k = w/v; // k = curvature
    // scale curvature by 50.
    k *= 20;
    double lambda = 2;
    double beta = 0.7;
    double v_curve = MAX_BOT_SPEED/(1+beta*pow(fabs(k),lambda));
    if (v_curve < MIN_BOT_SPEED)
        v_curve = MIN_BOT_SPEED;    
    v *= Pose::ticksToCmS;
    vl = v - Pose::d*w/2;
    vr = v + Pose::d*w/2;
    double timeMs = 0.250*rho + 14.0 * sqrt(rho) + 100.0 * fabs(gamma);
    double speed = timeMs/timeLCMs<(prevSpeed/MAX_BOT_LINEAR_VEL_CHANGE)?prevSpeed-MAX_BOT_LINEAR_VEL_CHANGE:prevSpeed+MAX_BOT_LINEAR_VEL_CHANGE;    
    // use vcurve as the velocity
    // NOTE: adding vcurve and finalVel code
    // critical condition: if bot close to final point, v_curve = MAX_BOT_SPEED
    if (rho < BOT_POINT_THRESH && finalSpeed > MIN_BOT_SPEED) {
        v_curve = MAX_BOT_SPEED;
        vl = vr; // ? or vl = vr?
    }
    double rangeMin = max(prevSpeed - MAX_BOT_LINEAR_VEL_CHANGE, 0.0);
    double rangeMax = min(prevSpeed + MAX_BOT_LINEAR_VEL_CHANGE, v_curve);
    if (v_curve < prevSpeed - MAX_BOT_LINEAR_VEL_CHANGE) {
        rangeMin = rangeMax = v_curve;
    }
    // only consider finalSpeed when d < BOT_FINALVEL_THRESH
    if (rho <= BOT_FINALVEL_THRESH)
        speed = finalSpeed < rangeMax? max(rangeMin, finalSpeed) : min(rangeMax, finalSpeed);
    else
        speed = rangeMax;
    // critical condition : finalVel = 0, but bot not close to final pose
    if (rho > BOT_POINT_THRESH && speed < MIN_BOT_SPEED)
        speed = MIN_BOT_SPEED;

    // end
    if(speed > MAX_BOT_SPEED)
        speed = MAX_BOT_SPEED;
    else if (speed < 0)
        speed = 0;
    double max = abs(vl)>abs(vr)?abs(vl):abs(vr);
    if(max > 0) {
        vl = vl*speed/max;
        vr = vr*speed/max;
    }
    // MiscData(k, v_curve, finalSpeed, rangeMin, rangeMax);
	// sprintf(debug,"vl : %d , vr : %d \n",vl,vr);
    // Client::debugClient->SendMessages(debug);
#if FIRA_COMM || FIRASSL_COMM
    comm->sendCommand(botID, vl, vr);
#else
    comm->sendCommand(botID, vl, vr);
#endif
    return; 
}

} // namespace MyStrategy
