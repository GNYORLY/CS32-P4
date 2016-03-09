#include "DiskMultiMap.h"
#include "BinaryFile.h"

////////////////////////Multi-Map//////////////////////////////

DiskMultiMap::DiskMultiMap() {}
DiskMultiMap::~DiskMultiMap() { close(); }

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets) 
{
	close();
	bool success = bf.createNew(filename);
	if (!success)
		return false;//cout << "Error! Unable to create myfile.dat\n";
	else
		cout << "Successfully created file myfile.dat\n";
	bf.write(header, 0);
	bf.write(numBuckets, 0);
	for (int i = 0; i < numBuckets; i++)
		bf.write(bucket, header + (bucket * i));
	return true;
}

bool DiskMultiMap::openExisting(const std::string& filename) 
{
	close();
	bool success = bf.openExisting(filename);
	if (!success)
		return false;//cout << "Error! Unable to find myfile.dat\n";
	else
		return true;// cout << "Successfully opened existing file myfile.dat\n";
}

void DiskMultiMap::close() 
{
	if (bf.isOpen())
		bf.close();
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context) 
{
	if (key.size() > 120)
		return false; //cout an error
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key) {}
int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context) {}




//////////////////////Iterator/////////////////////////////////

DiskMultiMap::Iterator::Iterator() {}
// You may add additional constructors
bool DiskMultiMap::Iterator::isValid() const {}
DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++() {}
MultiMapTuple DiskMultiMap::Iterator::operator*() {}
