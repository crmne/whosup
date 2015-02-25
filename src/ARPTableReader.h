#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

class ARPTableReader
{
	std::string path;
public:
	std::vector<std::string> ips;
	ARPTableReader(std::string iface, std::string p="/proc/net/arp");
};
