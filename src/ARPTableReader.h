#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

class ARPTableReader
{
	string path;
public:
	vector<string> ips;
	ARPTableReader(string iface, string p="/proc/net/arp");
};
