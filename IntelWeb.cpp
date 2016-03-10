#include "IntelWeb.h"
#include "BinaryFile.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

//////////////////////////////IntelWeb//////////////////////////////////

IntelWeb::IntelWeb() {}
IntelWeb::~IntelWeb() { close(); }

bool IntelWeb::createNew(const std::string& filePrefix, unsigned int maxDataItems) 
{ 
	close();
	int hashSize = 270000; // maxDataItems / .75;    //maxDataItems/.75 <= x or less

	if (dl.createNew(filePrefix + "_dl.dat", hashSize) == true &&
		create.createNew(filePrefix + "_create.dat", hashSize) == true &&
		contact.createNew(filePrefix + "_contact.dat", hashSize) == true)
		return true;
	else
		cout << "could not create new\n";
	return false;	
}

bool IntelWeb::openExisting(const std::string& filePrefix) 
{ 
	close();
	if (dl.openExisting(filePrefix + "_dl.dat") == true &&
		create.openExisting(filePrefix + "_create.dat") == true &&
		contact.openExisting(filePrefix + "_contact.dat") == true)
		return true;
	else
		cout << "could not open\n";
	return false;
}

void IntelWeb::close() 
{
	dl.close();
	create.close();
	contact.close();
}

bool IntelWeb::ingest(const std::string& telemetryFile) 
{ 
	string line;
	string first;
	string secnd;
	string third;
					                     
	ifstream file(telemetryFile);
	char s[121];
	
	while (getline(file, line))
	{
		int i = 0;
		for (i; i < strlen(line.c_str()); i++)
		{
			if (line[i] == ' ')
			{
				s[i] = '\0';
				break;
			}
			s[i] = line[i];
		}
		i++;
		first = s;
		int j = 0;
		for (i; i < strlen(line.c_str()); i++, j++)
		{
			if (line[i] == ' ')
			{
				s[j] = '\0';
				break;
			}
			s[j] = line[i];
		}
		i++;
		secnd = s;
		j = 0;
		for (i; i < strlen(line.c_str()); i++, j++)
		{
			if (line[i] == ' ')
			{
				s[j] = '\0';
				break;
			}
			s[j] = line[i];
		}
		s[j] = '\0';
		third = s;
		if (secnd.find(".exe") != std::string::npos)
		{
			if (third.find(".exe") != std::string::npos)
				create.insert(first, secnd, third);
			else if (third.find("http://") != std::string::npos)
				contact.insert(first, secnd, third);
		}
		else if (secnd.find("http://") != std::string::npos)
			dl.insert(first, secnd, third);
	}
	return true;	
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
							 unsigned int minPrevalenceToBeGood,
							 std::vector<std::string>& badEntitiesFound,
							 std::vector<InteractionTuple>& badInteractions) 
{
	return 1;
}

bool IntelWeb::purge(const std::string& entity) 
{ return false; }
