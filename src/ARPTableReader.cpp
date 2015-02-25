#include "ARPTableReader.h"

ARPTableReader::ARPTableReader(std::string iface, std::string p) {
    path = p;

    std::string line;
    std::ifstream arp_table(path.c_str());
    std::getline(arp_table, line); // ignore first line
    if (!arp_table.is_open()) {
        std::cerr << "Error while opening file." << std::endl;
        exit(1);
    }

    while (std::getline(arp_table, line) && arp_table.good()) {
        ;
        std::stringstream ss(line);
        std::string ip, dummy, interface;
        ss >> ip >> dummy >> dummy >> dummy >> dummy >> interface;
        if (iface.compare(interface) == 0) {
            ips.push_back(ip);
        }
    }
    arp_table.close();
}
