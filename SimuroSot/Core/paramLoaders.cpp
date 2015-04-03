#include "stdafx.h"
#include "paramLoaders.h"
#include "../Skills/skillSet.h"
#include "dataBox.h"
#include <fstream>
using namespace std;
namespace MyStrategy
{

  ParamLoaders::ParamLoaders()
  {
  }

  ParamLoaders::~ParamLoaders()
  {
  }
  
  void ParamLoaders::load_all_params()
  {
    skill_gotopoint();
  }
  
  void ParamLoaders::skill_gotopoint()
  {
    SkillSet::GoToPoint_profileFactorMult = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "profileFactorMult");
    SkillSet::GoToPoint_omegaFactor       = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "omegaFactor");
    SkillSet::Bot_Point_dis_penalty_l1    = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "botPointDisPenaltyL1");
    SkillSet::Bot_Point_dis_penalty_l2    = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "botPointDisPenaltyL2");
    SkillSet::angle_penalty_l1            = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "anglePenaltyL1");
    SkillSet::angle_penalty_l2            = DataBox::getDouble(SkillSet::skillsCollection + SkillSet::goToPointCollection, "anglePenaltyL2");
    SkillSet::dribble_ball_threshold      = DataBox::getDouble("Trainer.GotoBall", "dribble_ball_threshold");
    SkillSet::bot_ball_threshold          = DataBox::getDouble("Trainer.GotoBall", "bot_ball_threshold");
    ifstream fin((DataBox::dataDir + DataBox::getString("Trainer.GotoPoint", "serialized_pfacfile")).c_str(), ios::binary);
    deserialize(SkillSet::gotopoint_profilefac, fin);
    fin.close();
    fin.open((DataBox::dataDir + DataBox::getString("Trainer.GotoPoint", "serialized_ofacfile")).c_str(), ios::binary);
    deserialize(SkillSet::gotopoint_omegafac, fin);
    fin.close();
  }
}
