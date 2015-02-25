#include "ARPTableReader.h"

ARPTableReader::ARPTableReader(std::string iface, std::string p) {
    path = p;

    std::string line;
    std::ifstream arp_table(path.c_str());
    std::getline(arp_table, line); // ignore first line
    if (arp_table.is_open()) {
        while (getline(arp_table, line)) {
            std::cout << line << std::endl;
            std::stringstream ss(line);
            std::string ip, dummy, interface;
            ss >> ip >> dummy >> dummy >> dummy >> dummy >> interface;
            std::cout << ip << " -> " << interface << std::endl;
            if (iface.compare(interface) == 0) {
                ips.push_back(ip);
            }
        }
        arp_table.close();
    }
}
