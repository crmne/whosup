#include <iostream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <crafter.h>
#include <crafter/Utils/IPResolver.h>

using namespace std;
using namespace Crafter;

unsigned long max_threads = 254;  // TODO: more elegantly, we should query for the max number of BPF devices we can use

void usage() {
    cout << "Usage: whosup -i interface -a addresses [-q] [-t threads] [-r retries] [-w timeout]" << endl;
    exit(1);
}

int main(int argc, char *const *argv) {

    string iface, addresses;
    bool quiet = false;
    int threads = 0;
    int retries = 2;
    double timeout = 1.0;

    int c;
    extern char *optarg;
    extern int optind, optopt;
    if (argc < 5) usage();
    while((c =  getopt(argc, argv, ":i:a:qt:r:w:")) != EOF) {
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
            cerr << "Option -" << (char)optopt << " requires an operand" << endl;
            usage();
        case '?':
            cerr << "Unrecognised option: -" << (char)optopt << endl;
            usage();
        }
    }

    string MyIP = GetMyIP(iface);
    string MyMAC = GetMyMAC(iface);

    Ethernet ether_header;
    ether_header.SetSourceMAC(MyMAC);
    ether_header.SetDestinationMAC("ff:ff:ff:ff:ff:ff");
    ARP arp_header;
    arp_header.SetOperation(ARP::Request);
    arp_header.SetSenderIP(MyIP);
    arp_header.SetSenderMAC(MyMAC);

    vector<string> net = GetIPs(addresses);
    vector<string>::iterator ip_addr;
    vector<Packet*> request_packets;
    for(ip_addr = net.begin() ; ip_addr != net.end() ; ip_addr++) {
        arp_header.SetTargetIP(*ip_addr);
        Packet* packet = new Packet;
        packet->PushLayer(ether_header);
        packet->PushLayer(arp_header);
        request_packets.push_back(packet);
    }

    vector<Packet*> replies_packets(request_packets.size());
    if (threads == 0) {
        threads = min(net.size(), max_threads);
    }
    SendRecv(request_packets.begin(), request_packets.end(), replies_packets.begin(), iface, timeout, retries, threads);

    vector<Packet*>::iterator it_pck;
    if (!quiet) {
        cout << setfill(' ') << setw(18) << left << "Host" << setw(42) << left << "Hostname" << setw(18) << left << "MAC Address" << endl;
    }
    int counter = 0;
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++) {
        Packet* reply_packet = (*it_pck);
        if(reply_packet) {
            ARP* arp_layer = reply_packet->GetLayer<ARP>();
            if (! quiet) {
                cout << setfill(' ') << setw(18) << left << arp_layer->GetSenderIP() << setw(42) << left << GetHostname(arp_layer->GetSenderIP()) << setw(18) << left << arp_layer->GetSenderMAC() << endl;
            }
            counter++;
        }
    }

    cout << counter << " hosts up." << endl;

    for(it_pck = request_packets.begin() ; it_pck < request_packets.end() ; it_pck++)
        delete (*it_pck);
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++)
        delete (*it_pck);

    return 0;
}
