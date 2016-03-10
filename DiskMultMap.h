#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"

class DiskMultiMap
{
public:

	class Iterator
	{
	public:
		Iterator();
		Iterator(int bckt, int nodes, BinaryFile& BF);
		
		bool isValid() const;
		Iterator& operator++();
		MultiMapTuple operator*();

	private:
		BinaryFile* bf;
		bool m_valid;
		int m_nodes;
		int m_prev;
		int m_curr;
	};

	DiskMultiMap();
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename);
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context);
	Iterator search(const std::string& key);
	int erase(const std::string& key, const std::string& value, const std::string& context);

private:
	BinaryFile bf;
	BinaryFile::Offset header = 500;
	BinaryFile::Offset bucket = 50;
	BinaryFile::Offset nodeSpace = 400; //363
	string fileName;
	int hash(const string& str);
	void emptyNode(int space);
};

#endif // DISKMULTIMAP_H_
