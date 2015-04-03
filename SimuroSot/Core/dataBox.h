#pragma once
#ifndef DATABOX_H
#define DATABOX_H
#include <map>
#include <string>
namespace MyStrategy
{
  class DataBox
  {
  public:
    DataBox();
    ~DataBox();
    static std::string dataDir;
    static std::string dataFile;
    static bool loaded;
    static std::map<std::string, std::string> data;
    static void loadData(std::string fileName = "");
    static void saveData(std::string fileName = "");
    static void addData(std::string collectionName, std::string name, double value);
    static double getDouble(std::string collectionName, std::string name);
    static std::string getString(std::string collectionName, std::string name);;
  };
}
#endif // DATABOX_H
