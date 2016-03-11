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

	if (forward.createNew(filePrefix + "_forward.dat", hashSize) == true && backward.createNew(filePrefix + "_backward.dat", hashSize) == true)
		return true;
	else
		cout << "could not create new\n";
	return false;	
}

bool IntelWeb::openExisting(const std::string& filePrefix) 
{ 
	close();
	if (forward.openExisting(filePrefix + "_dl.dat") == true && backward.openExisting(filePrefix + "_create.dat") == true)
		return true;
	else
		cout << "could not open\n";
	return false;
}

void IntelWeb::close() 
{
	forward.close();
	backward.close();
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
		for (i; i < signed (strlen(line.c_str())); i++)
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
		for (i; i < signed (strlen(line.c_str())); i++, j++)
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
		for (i; i < signed (strlen(line.c_str())); i++, j++)
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
		
		unique.insert(secnd);
		unique.insert(third);
		malKeys.push(secnd);
		malKeys.push(third);
		forward.insert(secnd, third, first);		
		backward.insert(third, secnd, first);
	}
	return true;	
}

unsigned int IntelWeb::crawl(const std::vector<std::string>& indicators,
	unsigned int minPrevalenceToBeGood,
	std::vector<std::string>& badEntitiesFound,
	std::vector<InteractionTuple>& badInteractions)
{
	set<string>::iterator it;
	for (it = unique.begin(); it != unique.end(); it++)
		prevalence[*it] = 0;

	unique.clear();

	while (malKeys.empty() == false)
	{
		string keys = malKeys.front();
		malKeys.pop();
		int count = 1 + prevalence[keys];
		prevalence.erase(keys);
		prevalence[keys] = count;
	}

	for (int i = 0; i < signed(indicators.size()); i++)
		malKeys.push(indicators[i]);

	while (malKeys.empty() == false)
	{
		string mal = malKeys.front();
		malKeys.pop();
		prevalence.erase(mal);
		prevalence[mal] = 0;
		unique.insert(mal);
		DiskMultiMap::Iterator f = forward.search(mal);
		DiskMultiMap::Iterator b = backward.search(mal);
		while (f.isValid() == true)
		{
			if (unsigned(prevalence[(*f).value]) != 0 && unsigned(prevalence[(*f).value]) < minPrevalenceToBeGood)
			{
				prevalence.erase((*f).value);
				prevalence[(*f).value] = 0;
				malKeys.push((*f).value);
			}
			log.insert(InteractionTuple(mal, (*f).value, (*f).context));
			++f;
		}

		while (b.isValid() == true)
		{
			if (unsigned(prevalence[(*b).value]) != 0 && unsigned(prevalence[(*b).value]) < minPrevalenceToBeGood)
			{
				prevalence.erase((*b).value);
				prevalence[(*b).value] = 0;
				malKeys.push((*b).value);
			}
			log.insert(InteractionTuple((*b).value, mal, (*b).context));
			++b;
		}
	}

	int entities = 0;
	for (it = unique.begin(); it != unique.end(); it++)
	{
		badEntitiesFound.push_back(*it);
		entities++;
	}

	set<InteractionTuple>::iterator iter;
	for (iter = log.begin(); iter != log.end(); iter++)
	{
		badInteractions.push_back(*iter);
	}

	return entities;
}

bool IntelWeb::purge(const std::string& entity) 
{ 
	DiskMultiMap::Iterator f = forward.search(entity);
	DiskMultiMap::Iterator b = backward.search(entity);
	if (f.isValid() == false && b.isValid() == false)
		return false;

	while (f.isValid() == true)
	{
		forward.erase((*f).key, (*f).value, (*f).context);
		++f;
	}
	while (b.isValid() == true)
	{
		forward.erase((*b).value, (*b).key, (*b).context);
		++f;
	}
	
	return false; 
}


//////////////////////////////InteractionTuple Comparison Operator////////////////////////////////

inline
bool operator<(const InteractionTuple& left, const InteractionTuple& right)
{
	if (left.context < right.context)
		return true;
	return false;
}

inline
bool operator==(const InteractionTuple& left, const InteractionTuple& right)
{
	if (left.from == right.from && left.to == right.to && left.context == right.context)
		return true;
	return false;
}
