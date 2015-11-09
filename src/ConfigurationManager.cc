// C C++ includes
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sstream>
// KeyValueClusterPerf includes
#include "ConfigurationManager.h"
#include "logger.h"

map<string, string> ConfigurationManager::readFile(string fileName)
{
  // Open and read the file
  ifstream file(fileName);
  stringstream ss;
  ss << file.rdbuf();
  // Parse the file-string into a map<string, string>
  return readString(ss.str());
}

void ConfigurationManager::writeFile(string fileName, map<string, string> configuration)
{
  ofstream file;
  file.open(fileName);
  file << writeString(configuration);
  file.close();
}

map<string, string> ConfigurationManager::readString(string str)
{
  // predefine container for result
  map<string, string> configuration;
  size_t splitPosition;
  // split string line by line
  istringstream iss(str);
  string line;
  while (getline(iss, line)) {
    // if the line starts with '#' it's a comment, ignore it
    if (line.compare(0, 1, "#") != 0) {
      // split line on '=' and add to map
      if ((splitPosition = line.find("=")) != string::npos) {
        configuration[line.substr(0, splitPosition)] = line.substr(splitPosition + 1, string::npos);
      }
    }
  }
  // return final result
  return configuration;
}

string ConfigurationManager::writeString(map<string, string> configuration)
{
  stringstream ss;
  // iterate over entries in configuration map, add the first one seperately to prevent an extra newline
  map<string, string>::iterator it = configuration.begin();
  ss << it->first << "=" << configuration[it->first];
  it++;
  for (; it != configuration.end(); it++) {
    ss << endl << it->first << "=" << configuration[it->first];
  }
  // return the created string
  return ss.str();
}

list<string> ConfigurationManager::readHostFile(string fileName)
{
  list<string> hostList;
  // read file
  ifstream file(fileName);
  stringstream ss;
  ss << file.rdbuf();
  string str = ss.str();
  // as long as there are lines, add them to the list
  istringstream iss(str);
  string line;
  while (getline(iss, line)) {
    hostList.push_back(line);
  }
  return hostList;
}