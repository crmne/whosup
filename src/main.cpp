#include <iostream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <crafter.h>
#include <crafter/Utils/IPResolver.h>
#include "ARPTableReader.h"

using namespace Crafter;

unsigned long max_threads = 48;  // TODO: more elegantly, we should query for the max number of BPF devices we can use

void usage() {
    std::cout << "Usage: whosup -i interface [-a addresses] [-q] [-f arp_file] [-t threads] [-r retries] [-w timeout]" << std::endl;
    exit(1);
}

int main(int argc, char *const *argv) {

    std::string iface, addresses, arp_file;
    bool quiet = false;
    bool read_arp = false;
    int threads = 0;
    int retries = 2;
    double timeout = 1.0;

    int c;
    extern char *optarg;
    extern int optind, optopt;
    if (argc < 3) usage();
    while((c =  getopt(argc, argv, ":i:a:qf:t:r:w:")) != EOF) {
        switch (c) {
        case 'i':
            iface = optarg;
            break;
        case 'a':
            addresses = optarg;
            break;
        case 'q':
            quiet = true;
            break;
        case 'f':
            read_arp = true;
            arp_file = optarg;
            break;
        case 't':
            threads = atoi(optarg);
            break;
        case 'r':
            retries = atoi(optarg);
            break;
        case 'w':
            timeout = atof(optarg);
            break;
        case ':':
            std::cerr << "Option -" << (char)optopt << " requires an operand" << std::endl;
            usage();
        case '?':
            std::cerr << "Unrecognised option: -" << (char)optopt << std::endl;
            usage();
        }
    }

    std::string MyIP = GetMyIP(iface);
    std::string MyMAC = GetMyMAC(iface);

    Ethernet ether_header;
    ether_header.SetSourceMAC(MyMAC);
    ether_header.SetDestinationMAC("ff:ff:ff:ff:ff:ff");
    ARP arp_header;
    arp_header.SetOperation(ARP::Request);
    arp_header.SetSenderIP(MyIP);
    arp_header.SetSenderMAC(MyMAC);

    std::vector<std::string> net;
    if (read_arp) {
        ARPTableReader atr = ARPTableReader(iface, arp_file);
        net = atr.ips;
    } else {
        net = GetIPs(addresses);
    }
    std::vector<std::string>::iterator ip_addr;
    std::vector<Packet*> request_packets;
    for(ip_addr = net.begin(); ip_addr != net.end(); ip_addr++) {
        arp_header.SetTargetIP(*ip_addr);
        Packet* packet = new Packet;
        packet->PushLayer(ether_header);
        packet->PushLayer(arp_header);
        request_packets.push_back(packet);
    }

    std::vector<Packet*> replies_packets(request_packets.size());
    if (threads == 0) {
        threads = std::min((unsigned long) net.size(), max_threads);
    }
    SendRecv(request_packets.begin(), request_packets.end(), replies_packets.begin(), iface, timeout, retries, threads);

    std::vector<Packet*>::iterator it_pck;
    if (!quiet) {
        std::cout << std::setfill(' ') << std::setw(18) << std::left << "Host" << std::setw(42) << std::left << "Hostname" << std::setw(18) << std::left << "MAC Address" << std::endl;
    }
    int counter = 0;
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++) {
        Packet* reply_packet = (*it_pck);
        if(reply_packet) {
            ARP* arp_layer = reply_packet->GetLayer<ARP>();
            if (! quiet) {
                std::cout << std::setfill(' ') << std::setw(18) << std::left << arp_layer->GetSenderIP() << std::setw(42) << std::left << GetHostname(arp_layer->GetSenderIP()) << std::setw(18) << std::left << arp_layer->GetSenderMAC() << std::endl;
            }
            counter++;
        }
    }

    std::cout << counter << " hosts up." << std::endl;

    for(it_pck = request_packets.begin() ; it_pck < request_packets.end() ; it_pck++)
        delete (*it_pck);
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++)
        delete (*it_pck);

    return 0;
}
