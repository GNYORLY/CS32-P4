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
	fileName = filename;
	return true;
}

bool DiskMultiMap::openExisting(const std::string& filename) 
{
	close();
	bool success = bf.openExisting(filename);
	if (!success)
		return false;//cout << "Error! Unable to find myfile.dat\n";
	else
	{
		fileName = filename;
		return true;
	}                // cout << "Successfully opened existing file myfile.dat\n";
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
	char s[121];
	int i = hash(key);
	int numNodes;
	int emptyNodes;
	bf.read(emptyNodes, 20);
	bf.read(numNodes, header + (bucket*i));
	
	if (emptyNodes == 0)
    {
		bf.write(numNodes + 1, header + (bucket*i));
		int last;
		bf.read(last, 10);
		bf.write(nodeSpace, last);
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

		strcpy_s(s, key.c_str());
		bf.write(s, last);
		strcpy_s(s, value.c_str());
		bf.write(s, last + 121);
		strcpy_s(s, context.c_str());
		bf.write(s, last + 242);
		bf.write(last + nodeSpace, 10);
		return true;
	}
	else
	{
		int node;
		
		bf.write(numNodes + 1, header + (bucket*i));
		bf.read(node, 30 + (10*(emptyNodes - 1)));//use an empty one fr header
		
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
		
		strcpy_s(s, key.c_str());
		bf.write(s, node); 
		strcpy_s(s, value.c_str());
		bf.write(s, node + 121); 
		strcpy_s(s, context.c_str());
		bf.write(s, node + 242);
		bf.write(emptyNodes - 1, 20);
		return true; 
	}
	return false;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key) 
{
	int i = hash(key);
	int numNodes; 
	int pos = header + (bucket*i); 
	bf.read(numNodes, pos); 	
	if (numNodes > 0)
	{
		Iterator iter(pos, numNodes, bf);
		return iter;
	}
	else 
		cout << "not here\n";
	Iterator iter(0, 0, bf);
	return iter;
}

int DiskMultiMap::erase(const std::string& key, const std::string& value, const std::string& context)
{
	Iterator iter = search(key);
	int erased = 0;
	int i = hash(key);
	int numNodes = 0;
	int prev = header + (bucket*i);
	bf.read(numNodes, prev);
	prev += 25;
	//cout << numNodes<<"\n";
	for (int k = 0; k != numNodes; k++, ++iter)
	{
		if ((*iter).value == value && (*iter).context == context)
		{
			int erase;
			bf.read(erase, prev); 
			emptyNode(erase);
			if (k != (numNodes - 1))
			{
				bf.read(erase, erase + 365); 
				bf.write(erase, prev); 
			}
			erased++;
		}
		else
		{
			bf.read(prev, prev);
			prev += 365;
		}
	}
	openExisting(fileName);
	bf.write(numNodes - erased, header + (bucket*i)); 
	return erased;
}


////////////////////////////private////////////////////////////
int DiskMultiMap::hash(const string& str)
{
	int i;
	bf.read(i, 0);
	std::hash<std::string> str_hash; 
	unsigned int hashValue = str_hash(str);
	unsigned int buck = hashValue % i; 
	return buck;
	
}

void DiskMultiMap::emptyNode(int space)
{
	int empty;
	bf.read(empty, 20);
	bf.write(space, 30 + (10 * empty));
	bf.write(empty + 1, 20);
}


//////////////////////Iterator/////////////////////////////////

DiskMultiMap::Iterator::Iterator() 
{
	m_valid = false;
}

DiskMultiMap::Iterator::Iterator(int bckt, int nodes, BinaryFile& BF)
{
	bf = &BF;
	int curr;
	m_prev = bckt + 25; //prev points to the location of the "pointer" to the next node
	m_nodes = nodes;
	m_valid = true;
	bf->read(curr, m_prev);
	m_curr = curr; //curr points to the beginning of the node
}

bool DiskMultiMap::Iterator::isValid() const 
{ 
	return m_valid; 
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++() 
{ 
	if (m_valid == false)
		return *this;
	if (m_nodes <= 0)
	{
		cout << "no nodes\n";
		m_valid = false;
		return *this;
	}
	m_prev = m_curr + 365;
	int next;
	bf->read(next, m_prev);
	m_curr = next;
	m_nodes--;
	return *this;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{ 
	MultiMapTuple x;
	char s[121];
	bf->read(s, m_curr);
	std::string r = s;
	x.key = r;
	bf->read(s, m_curr + 121);
	r = s;
	x.value = r;
	bf->read(s, m_curr + 242);
	r = s;
	x.context = r;
	return x;
}

