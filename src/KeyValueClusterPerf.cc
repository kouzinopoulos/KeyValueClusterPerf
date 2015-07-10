#include "logger.h"
#include "KeyValueDB.h"
#include "RamCloudKeyValueDB.h"

int main(int argc, char *argv[])
{
	LOG("Application started");
	KeyValueDB* testDB;
	testDB = new RamCloudKeyValueDB();
	testDB->putValue("TestKey", "TestValue");
	delete testDB;
}