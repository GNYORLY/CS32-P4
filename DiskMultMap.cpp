#include "DiskMultiMap.h"
#include <functional>

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
	unsigned int i;
	for (i = 0; i < numBuckets; i++)
	{
		bf.write(bucket, header + (bucket * i));
		bf.write(0, header + (bucket * i));      //nodes
	}
	bf.write(header + (bucket * i), 10); //last position
	bf.write(0, 20); // empty nodes
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
	if (key.size() > 120 || value.size() > 120 || context.size() > 120)
		return false; 

	int i = hash(key);
	int numNodes;
	int emptyNodes;
	bf.read(emptyNodes, 20);
	bf.read(numNodes, header + (bucket*i));
	MultiMapTuple m;
	m.key = key;
	m.value = value;
	m.context = context;
	if (emptyNodes == 0)
	{
		bf.write(numNodes + 1, header + (bucket*i));
		int last;
		bf.read(last, 10);
		if (numNodes == 0)
		{
			bf.write(last, header + (bucket*i) + 25); 
		}
		else
		{
			int copy;
			bf.read(copy, header + (bucket*i) + 25);
			bf.write(last, header + (bucket*i) + 25);
			bf.write(copy, last + 365);
		}
		bf.write(m, last);
		bf.write(last + 363, 10);
		return true;
	}
	else
	{
		int node;
		bf.write(numNodes + 1, header + (bucket*i));
		bf.read(node, 30 + (5*(emptyNodes - 1)));//use an empty one fr header
		if (numNodes == 0)
		{
			bf.write(node, header + (bucket*i) + 25); // insert where the new node will be, not last
		}
		else
		{
			int copy;
			bf.read(copy, header + (bucket*i) + 25);
			bf.write(node, header + (bucket*i) + 25);
			bf.write(copy, node + 365);
		}

		bf.write(m, node);
		bf.write(emptyNodes - 1, 20);
		return true;
	}
	return false;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key) 
{
	int i = hash(key);
	int numNodes = 0;
	int pos = header + (bucket*i);
	bf.read(numNodes, pos);
	
	if (numNodes > 0)
	{
		Iterator iter(pos, numNodes);
		return iter;
	}
	else
		cout << "not here\n";
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context) { return 1; }


////////////////////////////private////////////////////////////
int DiskMultiMap::hash(const string& str)
{
	int i = 0;
	bf.read(i, 0);
	std::hash<std::string> str_hash;
	unsigned int hashValue = str_hash(str);
	unsigned int buck = hashValue % i;
	return buck;
}


//////////////////////Iterator/////////////////////////////////

DiskMultiMap::Iterator::Iterator() 
{
	m_valid = false;
}

DiskMultiMap::Iterator::Iterator(int& bckt, int& nodes)
{
	bf.openExisting("kk");
	int curr;
	m_prev = bckt + 25; //prev points to the location of the "pointer" to the next node
	m_nodes = nodes;
	m_valid = true;
	bf.read(curr, m_prev);
	m_curr = curr; //curr points to the beginning of the node
}



// You may add additional constructors
bool DiskMultiMap::Iterator::isValid() const 
{ 
	return m_valid; 
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++() 
{ 
	m_prev = m_curr + 365;
	int next;
	bf.read(next, m_curr + 365);
	m_curr = next;
	return;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{ 
	MultiMapTuple x;
	bf.read(x, m_curr);
	return x;
}
