#pragma once
#include "stdafx.h"
#include "../Tactics/tactic.h"
#include "../Core/beliefState.h"
#include "../winDebugger/Client.h"

using namespace std;
#define D_CAPTURE_BALL 0
#define D_BLOCK_BALL   1
const int APPROACH_CLEARANCE = (BOT_BALL_THRESH +BOT_RADIUS);
#define sigmoid(x) (1./(1.+exp(-(x))))

namespace MyStrategy {
  // Functions which will be shifted to Utils later
  bool getTargetClearance(const vector<pair<float, float> > obstacles,
                          const float target, pair<float, float> &result) {
    vector<pair<float, float> >::const_iterator it = obstacles.begin();
    float start = obstacles.end()->second - 2*PI;
    while(it != obstacles.end() && it->first < target) {
      if(target < it->second) {
        result = *it;
        return false;
      }
      start = it->second;
      it++;
    }
    if(it == obstacles.end()) {
      result = pair<float, float>(obstacles.end()->second - 2*PI, obstacles.begin()->first);
    } else {
      result = pair<float, float>(start, it->first);
    }
    return true;
  }
  inline bool getIntersection(const pair<float, float> &range1, const pair<float, float> &range2, pair<float, float> &result) {
    result = pair<float, float>(max(range1.first, range2.first), min(range1.second, range2.second));
    return result.first < result.second;
  }
  
  inline void adjustRange(float &theta1, float &theta2) {
    if(abs(theta1-theta2) > PI) {
      if(abs(theta1) > abs(theta2)) {
        if(theta1 > 0) {
          theta1 -= 2*PI;
        } else {
          theta1 += 2*PI;
        }
      } else {
        if(theta2 > 0) {
          theta2 -= 2*PI;
        } else {
          theta2 += 2*PI;
        }
      }
    }
    if(theta1 > theta2)
      swap(theta1, theta2);
  }
  
  bool getIntersection(const vector<pair<float, float> > &obstacles, pair<float, float> &targetRange, 
                        pair<float, float> &result) {
    float start = obstacles.end()->second;
    start -= 2*PI;
    vector<pair<float, float> >::const_iterator it_cur  = obstacles.begin();
    pair<float, float> last_best(0,0); float last_max = 0;
    while(it_cur != obstacles.end()) {
      float end = it_cur->first;
      if(end - start > 0.01) {
        pair<float, float> intersection_res;
        if(getIntersection(targetRange, pair<float, float> (start, end), intersection_res)) {
          float delta = intersection_res.second - intersection_res.first;
          if(delta > last_max) {
            last_max  = delta;
            last_best = intersection_res;
          }
        }
      }
      start = it_cur->second;
      it_cur++;
    }
    float end = obstacles.begin()->first + 2*PI;
    if(end - start > 0.01) {
      pair<float, float> intersection_res;
      if(getIntersection(targetRange, pair<float, float> (start, end), intersection_res)) {
        float delta = intersection_res.second - intersection_res.first;
        if(delta > last_max) {
          last_max  = delta;
          last_best = intersection_res;
        }
      }
    }
    if(last_max < 0.01)
      return false;
    result = last_best;
    return true;
  }
  
  bool Tactic::getOppGoalClearance(const vector<pair<float, float> > &obstacles, const Vector2D<int> &center, 
                            pair<float, float> &result) const {
    Vector2D<int> end1(ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MAXY);
    Vector2D<int> end2(ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MINY);
    float theta1 = Vector2D<int>::angle(end1, center);
    float theta2 = Vector2D<int>::angle(end2, center);
    adjustRange(theta1, theta2);
    pair<float, float> goalrange(theta1, theta2);
    return getIntersection(obstacles, goalrange, result);
  }
  
  bool Tactic::getOurGoalClearance(const vector<pair<float, float> > &obstacles, const Vector2D<int> &center,
                            pair<float, float> &result) const {
    Vector2D<int> end1(-ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MAXY);
    Vector2D<int> end2(-ForwardX(HALF_FIELD_MAXX), OUR_GOAL_MINY);
    float theta1 = Vector2D<int>::angle(end1, center);
    float theta2 = Vector2D<int>::angle(end2, center);
    adjustRange(theta1, theta2);
    pair<float, float> goalrange(theta1, theta2);
    return getIntersection(obstacles, goalrange, result);
  }
  
  bool Tactic::getBotClearance(const vector<pair<float, float> > &obstacles, const Vector2D<int> &center,
                            pair<float, float> &result, const Vector2D<int> &oppPos) const {
    const float DELTA_ANGLE = 0.15;
    float theta  = Vector2D<int>::angle(oppPos, center);
    float theta1 = theta - DELTA_ANGLE;
    float theta2 = theta + DELTA_ANGLE;
    adjustRange(theta1, theta2);
    pair<float, float> goalrange(theta1, theta2);
    return getIntersection(obstacles, goalrange, result);
  }
  // List utility functions for use in common in all tactics here.
  bool Tactic::isBallInMyWideAngleRange() const {
    // TODO:: Implement or remove
    return false;
  }
  
  bool Tactic::isBallInMyWideAngleRange(const int botID) const {
    // TODO:: Implement or remove
    return false;
  }
  
  bool Tactic::isBallInMyLastHopeRange(const int botID) const {
    // TODO:: Implement or remove
    return false;
  }
  
  bool Tactic::isBallWithMe() const {
    // TODO: Review the conditions. Is this all or more sophistication required?
    return state->pr_ourBall && (state->ourBotNearestToBall == botID);
  }
  
  bool Tactic::isBallAchievableInForwardMotion() const {
    return (Vector2D<int>::dist(state->homePos[botID], state->ballPos) > BOT_RADIUS*4) ||
            (abs(normalizeAngle(Vector2D<int>::angle(state->ballPos, state->homePos[botID]) - state->homeAngle[botID])) < DRIBBLER_BALL_ANGLE_RANGE);
  }
  
  void Tactic::addsegment(vector<pair<float, float> > &obs, const Vector2D<int> &end1, 
                          const Vector2D<int> &end2, const Vector2D<int> &center) const {
    float theta1 = Vector2D<int>::angle(end1, center);
    float theta2 = Vector2D<int>::angle(end2, center);
    if(abs(theta1 - theta2) > PI) {
      if(theta1 > theta2) {
        obs.push_back(pair<float, float> (theta1, PI));
        obs.push_back(pair<float, float> (-PI, theta2));
      } else {
        obs.push_back(pair<float, float> (theta2, PI));
        obs.push_back(pair<float, float> (-PI, theta1));
      }
    } else {
      if(theta1 > theta2) {
        obs.push_back(pair<float, float> (theta2, theta1));
      } else {
        obs.push_back(pair<float, float> (theta1, theta2));
      }
    }
  }
  
  void Tactic::getFullAngularClearance(vector<pair<float, float> > &result, const int radius, const Vector2D<int> &center) const {
    // TODO: Some clearance gap needs to be added but how much?
    vector<pair<float, float> > obstacles;
    // adding obstacles for our bots
    for(int i = 0; i < HomeTeam::SIZE; i++) {
      if(i == botID) continue;
      if(Vector2D<int>::dist(state->homePos[i], center) >= radius) continue;
      float angle = Vector2D<int>::angle(state->homePos[i], center);
      Vector2D<int> end1(state->homePos[i].x + BOT_RADIUS*sin(angle), state->homePos[i].y - BOT_RADIUS*cos(angle));
      Vector2D<int> end2(state->homePos[i].x - BOT_RADIUS*sin(angle), state->homePos[i].y + BOT_RADIUS*cos(angle));
      float theta1 = Vector2D<int>::angle(end1, center);
      float theta2 = Vector2D<int>::angle(end2, center);
      if(theta1 > theta2) {
        obstacles.push_back(pair<float, float>(-PI, theta2));
        obstacles.push_back(pair<float, float>(theta1, PI));
      }else {
        obstacles.push_back(pair<float, float>(theta1, theta2));
      }
    }
    
    // adding obstacles for opp bots
    for(int i = 0; i < AwayTeam::SIZE; i++) {
      if(Vector2D<int>::dist(state->awayPos[i], state->homePos[botID]) > radius) continue;
      float angle = Vector2D<int>::angle(state->awayPos[i], state->homePos[botID]);
      Vector2D<int> end1(state->awayPos[i].x + BOT_RADIUS*sin(angle), state->awayPos[i].y - BOT_RADIUS*cos(angle));
      Vector2D<int> end2(state->awayPos[i].x - BOT_RADIUS*sin(angle), state->awayPos[i].y + BOT_RADIUS*cos(angle));
      float theta1 = Vector2D<int>::angle(end1, state->homePos[botID]);
      float theta2 = Vector2D<int>::angle(end2, state->homePos[botID]);
      if(theta1 > theta2) {
        obstacles.push_back(pair<float, float>(-PI, theta2));
        obstacles.push_back(pair<float, float>(theta1, PI));
      }else {
        obstacles.push_back(pair<float, float>(theta1, theta2));
      }
    }
    // adding obstacles for our goal
    Vector2D<int> ourg1(-HALF_FIELD_MAXX, OUR_GOAL_MINY);
    Vector2D<int> ourg2(-HALF_FIELD_MAXX - GOAL_DEPTH, OUR_GOAL_MINY);
    Vector2D<int> ourg3(-HALF_FIELD_MAXX - GOAL_DEPTH, OUR_GOAL_MAXY);
    Vector2D<int> ourg4(-HALF_FIELD_MAXX, OUR_GOAL_MAXY);
    if(Vector2D<int>::dist((ourg1+ourg2)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg1, ourg2, state->homePos[botID]);
    if(Vector2D<int>::dist((ourg2+ourg3)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg2, ourg3, state->homePos[botID]);
    if(Vector2D<int>::dist((ourg3+ourg4)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, ourg3, ourg4, state->homePos[botID]);

    // adding obstacles for opp goal
    Vector2D<int> oppg1(HALF_FIELD_MAXX, OUR_GOAL_MINY);
    Vector2D<int> oppg2(HALF_FIELD_MAXX + GOAL_DEPTH, OUR_GOAL_MINY);
    Vector2D<int> oppg3(HALF_FIELD_MAXX + GOAL_DEPTH, OUR_GOAL_MAXY);
    Vector2D<int> oppg4(HALF_FIELD_MAXX, OUR_GOAL_MAXY);
    if(Vector2D<int>::dist((oppg1+oppg2)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg1, oppg2, state->homePos[botID]);
    if(Vector2D<int>::dist((oppg2+oppg3)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg2, oppg3, state->homePos[botID]);
    if(Vector2D<int>::dist((oppg3+oppg4)/2, state->homePos[botID]) < radius)
      addsegment(obstacles, oppg3, oppg4, state->homePos[botID]);
    
    sort(obstacles.begin(), obstacles.end());
    
    float start = -PI, end = -PI - 1;
    vector<pair<float, float> >::iterator it = obstacles.begin();
    while(it != obstacles.end()) {
      if(it->first > end) {
        if(end > start) {
          result.push_back(pair<float, float> (start, end));
        }
        start = it->first;
      }
      end = max(end, it->second);
      it++;
    }
    result.push_back(pair<float, float> (start, end));
  }
  
  // List the stateless decisive skill calling functions here.
  void Tactic::kickBall(int speed) {
	//Nope! not there in FIRA
//    sID = SkillSet::Kick;
//    sParam.KickP.power = speed;
//    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::kickBallFullSpeed() {
    kickBall(MAX_KICK_SPEED);
  }
  void Tactic::kickBallForPass(const float distance) {
    kickBall(min(MAX_KICK_SPEED, (int)(MAX_KICK_SPEED/4 + distance/800)));
  }
  void Tactic::turnToTargetWithBall(const float goalAngle) {
	//This was earlier for SSL only, don't see relevance in FIRA
    // TODO improve this
    // Turn must be slow to avoid losing the ball
    sParam.TurnToAngleP.finalslope = goalAngle;
    skillSet->executeSkill(SkillSet::TurnToAngle, sParam);
  }
  void Tactic::preciseKick(float angle, int speed) {
    const int PRECISE_KICK_THRESHOLD = SELECT(0.2, 0.3);
    if(fabs((float)angle - state->homeAngle[botID]) > PRECISE_KICK_THRESHOLD) {
      turnToTargetWithBall(angle);
    } else {
      kickBall(speed);
    }
  }
  bool Tactic::canBlockBall(float &dist_from_line) const {
    float alpha; Vector2D<int> perpendicularBase;
    return get_perpendicular_base(state->ballPos, state->ballVel, state->homePos[botID], perpendicularBase, alpha);
  }
  void Tactic::moveToBlockBall() {
    const float leadTime = 1; // check for position after 1 second
    Vector2D<int> lead(leadTime * state->ballVel.x, leadTime * state->ballVel.y);
    Vector2D<int> targetPos = state->ballPos + lead;
    float alpha; Vector2D<int> perpendicularBase;
    float perpendicular_go = get_perpendicular_base(state->ballPos, state->ballVel, state->homePos[botID], perpendicularBase, alpha);
    if(perpendicular_go) {
      float finalSlope   = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
      ///Was goToPointQuick
      gotoPoint(perpendicularBase.x, perpendicularBase.y, true, finalSlope, 0);
    } else {
      ///Was goToPointQuick
      gotoPoint(targetPos.x, targetPos.y, false, 0, 0);
    }
  }
  void Tactic::gotoStaticBall(const float finalSlope) {
    float dist_from_ball = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
    float targetAngle    = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
    float angular_dist   = fabs((float)normalizeAngle( targetAngle - state->homeAngle[botID]));
    if(dist_from_ball < BOT_BALL_THRESH + 2*BOT_RADIUS) {
      if(angular_dist > DRIBBLER_BALL_ANGLE_RANGE + 0.1) {
        turnToAngle(targetAngle);
      } else  if(angular_dist > DRIBBLER_BALL_ANGLE_RANGE) {
        turnToAngle(targetAngle, MAX_BOT_OMEGA/2);
      } else if(dist_from_ball > BOT_BALL_THRESH + BOT_RADIUS) {
        gotoPoint(state->ballPos.x, state->ballPos.y, true, targetAngle, 0);
      } else {
        float angle = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
        gotoPoint(state->ballPos.x - BOT_RADIUS*cos(angle), state->ballPos.y - BOT_RADIUS*sin(angle), true, targetAngle, 0);
      }
    } else {
      float target_angular_seperation = normalizeAngle(targetAngle - finalSlope);
      if(fabs((float)target_angular_seperation) < PI/3)
        gotoPoint(state->ballPos.x, state->ballPos.y, true, finalSlope, 0);
      else {
        Vector2D<int> pos1(state->ballPos.x - APPROACH_CLEARANCE*cos(targetAngle-PI/3), state->ballPos.y - APPROACH_CLEARANCE*sin(targetAngle-PI/3));
        Vector2D<int> pos2(state->ballPos.x - APPROACH_CLEARANCE*cos(targetAngle+PI/3), state->ballPos.y - APPROACH_CLEARANCE*sin(targetAngle+PI/3));
        if(Vector2D<int>::dist(state->homePos[botID], pos1) < Vector2D<int>::dist(state->homePos[botID], pos2)) {
          gotoPoint(pos1.x, pos1.y, true, finalSlope, 0);
        } else {
          gotoPoint(pos2.x, pos2.y, true, finalSlope, 0);
        }
      }
    }
  }
  void Tactic::captureBall(const float targetAngle) {
    float ball_vel_sq = state->ballVel.absSq();
    float dist = Vector2D<int>::dist(state->ballPos, state->homePos[botID]);
   // //debug(D_CAPTURE_BALL, "dist to ball:", dist);
    if(ball_vel_sq < ZERO_VELOCITY_THRESHOLD_SQ) {
  //    //debug(D_CAPTURE_BALL, "going to static ball facing goal");
      gotoStaticBall(targetAngle);
    } if(ball_vel_sq < LOW_BALL_VELOCITY_THRES_SQ || dist < 2*BOT_BALL_THRESH) {
  //    //debug(D_CAPTURE_BALL, "going to static ball facing ball");
      float ballAngle = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
      gotoStaticBall(ballAngle);
    } else {
   //   //debug(D_CAPTURE_BALL, "blocking moving ball");
      moveToBlockBall();
    }
  }
  void Tactic::gotoPoint(int x, int y, bool align, float finalSlope, float finalVelocity) {
    sID = SkillSet::GoToPoint;
    sParam.GoToPointP.x                       = x ;
    sParam.GoToPointP.y                       = y ;
    sParam.GoToPointP.align                   = align;
    sParam.GoToPointP.finalslope              = finalSlope ;
    sParam.GoToPointP.finalVelocity           = finalVelocity;
    sParam.GoToPointP.bot_point_dist_penalty  = SkillSet::Bot_Point_dis_penalty_l2;
    sParam.GoToPointP.angle_diff_penalty      = SkillSet::angle_penalty_l2;
      
    // Execute the selected skill
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::gotoPointQuick(int x, int y, bool align, float finalSlope, float finalVelocity) {
    ///Was GoToPointQuick
	///Was here for SSL, don't see relevance for FIRA. maybe gotoPointStraight?
	assert(0);
//    sID = SkillSet::GoToPoint;
//    sParam.GoToPointQuickP.x                       = x ;
//    sParam.GoToPointQuickP.y                       = y ;
//    sParam.GoToPointQuickP.align                   = align;
//    sParam.GoToPointQuickP.finalSlope              = finalSlope ;
//    sParam.GoToPointQuickP.finalVelocity           = finalVelocity;
//    sParam.GoToPointQuickP.bot_point_dist_penalty  = SkillSet::Bot_Point_dis_penalty_l2;
//    sParam.GoToPointQuickP.angle_diff_penalty      = SkillSet::angle_penalty_l2;
//      
//    // Execute the selected skill
//    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::gotoPointExact(int x, int y, bool align, float finalSlope, float finalVelocity) {
    sID = SkillSet::GoToPoint;
    sParam.GoToPointP.x                       = x ;
    sParam.GoToPointP.y                       = y ;
    sParam.GoToPointP.align                   = align;
    sParam.GoToPointP.finalslope              = finalSlope ;
    sParam.GoToPointP.finalVelocity           = finalVelocity;
    sParam.GoToPointP.bot_point_dist_penalty  = SkillSet::Bot_Point_dis_penalty_l1;
    sParam.GoToPointP.angle_diff_penalty      = SkillSet::angle_penalty_l1;
      
    // Execute the selected skill
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::standAndDribble() {
	  //SSL only
    assert(0);
  }
  void Tactic::turnToAngle(const float finalSlope, const int maxOmega) {
    sID = SkillSet::TurnToAngle;
    sParam.TurnToAngleP.finalslope = finalSlope;
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::stopBot() {
    sID = SkillSet::Stop;
    skillSet->executeSkill(sID, sParam);
  }
  void Tactic::coverBall() {
    static const int COVER_DISTANCE = 4*BOT_RADIUS;
    int x1 = state->ballPos.x - COVER_DISTANCE, y1 = state->ballPos.y;
    if(abs(x1) > HALF_FIELD_MAXX) {
      if(x1 < 0) x1 = -HALF_FIELD_MAXX;
      else       x1 =  HALF_FIELD_MAXX;
      int curd = COVER_DISTANCE - x1 + state->ballPos.x;
      int dy = sqrt(COVER_DISTANCE*COVER_DISTANCE - curd*curd);
      if(y1 > 0) y1 -= dy;
      else       y1 += dy;
    }
    float angle = Vector2D<int>::angle(state->ballPos, state->homePos[botID]);
    gotoPoint(x1, y1, true, angle, 0);
  }
  int Tactic::selectBestBotforPassReceive(const Vector2D<int> passer, const int preferred_bot, const int passerid) const
  {
    const int OPTIMAL_PASS_DISTANCE = 100;
    const int PASS_SIGMOID_WIDTH    = 70;
    const int OPTIMAL_X_GAIN        = 250;
    int max_score = -1e9;
    int maxv = (passerid == 0)?0:1;
    for(int cur_bot = 0; cur_bot < HomeTeam::SIZE; cur_bot++)
    {
      if(cur_bot == passerid) continue;
      float source_dist = Vector2D<int>::dist(passer, state->homePos[cur_bot]);
      float goal_dist   = Vector2D<int>::dist(state->homePos[cur_bot], ForwardX(HALF_FIELD_MAXX), 0);
      vector<pair<float, float> > obs;
      getFullAngularClearance(obs, source_dist, passer);
      pair<float, float> target_bot;
      bool is_clear_bot = getBotClearance(obs, passer, target_bot, state->homePos[cur_bot]);
      obs.clear();
      getFullAngularClearance(obs, goal_dist, state->homePos[cur_bot]);
      pair<float, float> target_goal;
      bool is_clear_goal = getOppGoalClearance(obs, state->homePos[cur_bot], target_goal);
      float bot_dist = Vector2D<int>::dist(state->homePos[cur_bot], passer);
      float bot_clearance_score = ((is_clear_bot)?(normalizeAngle(target_bot.second - target_bot.first)):1e-5);
      float goal_clearance_score = ((is_clear_goal)?(normalizeAngle(target_goal.second - target_goal.first)):1e-5);
      float region_score = sigmoid((ForwardX(state->homePos[cur_bot].x - passer.x) - OPTIMAL_X_GAIN)/PASS_SIGMOID_WIDTH) * sigmoid((bot_dist-OPTIMAL_PASS_DISTANCE)/PASS_SIGMOID_WIDTH);
      float total_score  = bot_clearance_score*goal_clearance_score*region_score;
      if(cur_bot == preferred_bot) total_score*=1.1;
      if(total_score > max_score)
      {
        max_score = total_score;
        maxv = cur_bot;
      }
    }
    return maxv;
  }
  void Tactic::kickForGoalOrPassBall() {
    static int targetid = -1;
    targetid = selectBestBotforPassReceive(state->homePos[botID], targetid, botID);
    const Vector2D<int> &targetpos = state->homePos[targetid];
    const Vector2D<int> &mypos = state->homePos[botID];
    float distToBot = Vector2D<int>::dist(targetpos, mypos);
    float dist_to_goal = Vector2D<int>::dist(mypos, ForwardX(HALF_FIELD_MAXX), 0);
    vector<pair<float, float> > obs;
    getFullAngularClearance(obs, dist_to_goal, mypos);
    pair<float, float> goaltarget;
    bool is_goal_clear = getOppGoalClearance(obs, mypos, goaltarget);
    if(is_goal_clear) {
      float targetAngle = (goaltarget.first + goaltarget.second)/2;
      if(fabs((float)normalizeAngle(targetAngle - state->homeAngle[botID])) > min(max(abs(normalizeAngle(goaltarget.second - goaltarget.first))/10, 0.01f), 0.05f) ){
        turnToTargetWithBall(targetAngle);
      } else {
        kickBallFullSpeed();
        tState = COMPLETED;
      }
    }
    getFullAngularClearance(obs, distToBot, mypos);
    pair<float, float> target;
    bool ready_to_go = getBotClearance(obs, mypos, target, state->homePos[targetid]);
    /*
    SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y,
                            state->homePos[botID].x + 1000*cos(target.first),
                            state->homePos[botID].y + 1000*sin(target.first));
    SkillSet::comm->addLine(state->homePos[botID].x, state->homePos[botID].y,
                            state->homePos[botID].x + 1000*cos(target.second),
                            state->homePos[botID].y + 1000*sin(target.second));
    */
    if(ready_to_go) {
      float targetAngle = (target.first + target.second)/2;
      const float PASS_BALL_THRESHOLD = 0.02;
      if(fabs((float)normalizeAngle(targetAngle - state->homeAngle[botID])) > PASS_BALL_THRESHOLD) {
        turnToTargetWithBall(targetAngle);
      } else {
        kickBallForPass(Vector2D<int>::dist(targetpos, mypos));
        tState = COMPLETED;
      }
    } else {
      gotoPointExact(targetpos.x, targetpos.y, false, 0, 0);
    }
  }
}
