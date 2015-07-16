#ifndef __CONFIGURATIONMANAGER_H__
#define __CONFIGURATIONMANAGER_H__

#include <list>
#include <map>
#include <string>

using namespace std;

class ConfigurationManager
{
	public:
		map<string, string> readFile(string fileName);
		void writeFile(string fileName, map<string, string> configuration);
		map<string, string> readString(string str);
		string writeString(map<string, string> configuration);

		list<string> readHostFile(string fileName);
};

#endif