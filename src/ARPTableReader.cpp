#include "ARPTableReader.h"

ARPTableReader::ARPTableReader(string iface, string p) {
    path = p;

    string line;
    ifstream arp_table(path);
    getline(arp_table, line); // ignore first line
    if (arp_table.is_open()) {
        while (getline(arp_table, line)) {
            istringstream ss(line);
            string ip, dummy, interface;
            ss >> ip >> dummy >> dummy >> dummy >> dummy >> interface;
            if (iface.compare(interface) == 0) {
                ips.push_back(ip);
            }
        }
        arp_table.close();
    }
}
