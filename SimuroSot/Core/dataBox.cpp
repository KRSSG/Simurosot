#include "stdafx.h"
#include "dataBox.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <cstdlib>
using namespace std;
using namespace MyStrategy;
string DataBox::dataDir  = "../DataBox/";
string DataBox::dataFile = "data_box.txt";
map<string, string> DataBox::data;
DataBox::DataBox()
{
}

DataBox::~DataBox()
{
}

void DataBox::loadData(string fileName) {
  if(fileName == "") fileName = dataFile;
  else dataFile = fileName;
  fileName = dataDir + fileName;
  ifstream ifile(fileName.c_str(), ios::in);
  string key;
  while(ifile>>key) {
    string val; ifile>>val;
    data[key] = val;
    cout<<key<<":"<<val<<endl;
  }
  ifile.close();
  cout<<"file loaded"<<fileName<<endl;
}

void DataBox::addData(string collectionName, string name, double value) {
  char v[40];
  sprintf(v, "%lf", value);
  data[collectionName+"."+name] = string(v);
}



double DataBox::getDouble(string collectionName, string name) {
  string key = collectionName+"."+name;
  //assert(data.find(key) != data.end());
  return atof(data[key].c_str());
}

string DataBox::getString(string collectionName, string name) {
  string key = collectionName+"."+name;
  cout<<"Searching:"<<key<<endl;
  //assert(data.find(key) != data.end());
  return data[key];
}

void DataBox::saveData(string fileName) {
  cout<<"Saving Data..."<<endl;
  if(fileName == "") fileName = dataFile;
  ofstream ofile(fileName.c_str());
  for(map<string, string>::iterator it = data.begin(); it!=data.end(); it++)
  {
    ofile<<it->first<<" "<<it->second<<endl;
  }
  ofile.close();
}