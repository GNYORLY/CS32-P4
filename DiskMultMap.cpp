#include "DiskMultiMap.h"

////////////////////////Multi-Map//////////////////////////////

DiskMultiMap::DiskMultiMap() {}
DiskMultiMap::~DiskMultiMap() {}
bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets) {}
bool DiskMultiMap::openExisting(const std::string& filename) {}
void DiskMultiMap::close() {}
bool DiskMultiMap::insert(const std::string& key, const std::string& value, const std::string& context) {}
DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key) {}
int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context) {}




//////////////////////Iterator/////////////////////////////////

DiskMultiMap::Iterator::Iterator() {}
// You may add additional constructors
bool DiskMultiMap::Iterator::isValid() const {}
DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++() {}
MultiMapTuple DiskMultiMap::Iterator::operator*() {}
