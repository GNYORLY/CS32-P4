#include "IntelWeb.h"
#include "BinaryFile.h"

//////////////////////////////IntelWeb//////////////////////////////////

IntelWeb::IntelWeb() {}
IntelWeb::~IntelWeb() { close(); }

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems) 
{ 
	int hashSize = 2700000; // maxDataItems / .75;    //maxDataItems/.75 <= x or less

	dl.createNew(filePrefix + "_dl.dat", hashSize);
	create.createNew(filePrefix + "_create.dat", hashSize);
	contact.createNew(filePrefix + "_contact.dat", hashSize);
}

bool IntelWeb::openExisting(const std::string& filePrefix) 
{ 
	close();
	dl.openExisting(filePrefix + "_dl.dat");
	create.openExisting(filePrefix + "_create.dat");
	contact.openExisting(filePrefix + "_contact.dat");
}

void IntelWeb::close() 
{
	dl.close();
	create.close();
	contact.close();
}

bool IntelWeb::ingest(const std::string& telemetryFile) 
{ return false; }

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
							 unsigned int minPrevalenceToBeGood,
							 std::vector<std::string>& badEntitiesFound,
							 std::vector<InteractionTuple>& badInteractions) 
{
	return 1;
}

bool IntelWeb::purge(const std::string& entity) 
{ return false; }
