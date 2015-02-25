#include "ARPTableReader.h"

ARPTableReader::ARPTableReader(std::string iface, std::string p) {
    path = p;

    std::string line;
    std::ifstream arp_table(path.c_str());
    std::getline(arp_table, line); // ignore first line
    if (arp_table.is_open()) {
        while (getline(arp_table, line)) {
            char ip[17], interface[17], dummy[100];
            sscanf(line.c_str(), "%s %s %s %s %s %s", ip, dummy, dummy, dummy, dummy, interface);
            if (strcmp(iface.c_str(), interface) == 0) {
                std::string s(ip);
                ips.push_back(s);
            }
        }
        arp_table.close();
    }
}
