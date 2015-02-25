#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <crafter.h>
#include "ARPTableReader.h"

using namespace Crafter;

unsigned long max_threads = 48;  // TODO: more elegantly, we should query for the max number of BPF devices we can use

int connected_hosts(std::string iface, std::string path, int threads, int retries, double timeout) {
    std::string MyIP = GetMyIP(iface);
    std::string MyMAC = GetMyMAC(iface);

    Ethernet ether_header;
    ether_header.SetSourceMAC(MyMAC);
    ether_header.SetDestinationMAC("ff:ff:ff:ff:ff:ff");
    ARP arp_header;
    arp_header.SetOperation(ARP::Request);
    arp_header.SetSenderIP(MyIP);
    arp_header.SetSenderMAC(MyMAC);

    std::vector<std::string> net = ARPTableReader(iface, path).ips;
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
	int counter = 0;
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++) {
        Packet* reply_packet = (*it_pck);
        if(reply_packet)
            counter++;
    }

	for(it_pck = request_packets.begin() ; it_pck < request_packets.end() ; it_pck++)
        delete (*it_pck);
    for(it_pck = replies_packets.begin() ; it_pck < replies_packets.end() ; it_pck++)
        delete (*it_pck);

    return counter;
}

void usage() {
    std::cout << "Usage: whosup_collectd -i interface -f arp_file [-t threads] [-r retries] [-w timeout]" << std::endl;
    exit(1);
}

int main(int argc, char *const *argv)
{
	char *hostname, *interval_s;
	hostname = getenv("COLLECTD_HOSTNAME");
	if (hostname == NULL) {
		std::cerr << "COLLECTD_HOSTNAME environment variable not set." << std::endl;
		abort();
	}

	interval_s = getenv("COLLECTD_INTERVAL");
	if (interval_s == NULL) {
		std::cerr << "COLLECTD_INTERVAL environment variable not set." << std::endl;
		abort();
	}
	float interval = atof(interval_s);

	std::string iface, arp_file;
	int threads = 0;
    int retries = 2;
    double timeout = 1.0;

    int c;
    extern char *optarg;
    extern int optind, optopt;
    if (argc < 3) usage();
    while((c =  getopt(argc, argv, ":i:f:t:r:w:")) != EOF) {
        switch (c) {
        case 'i':
            iface = optarg;
            break;
        case 'f':
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

	while (true) {
		int n_connected = connected_hosts(iface, arp_file, threads, retries, timeout); 
		std::cout << "PUTVAL " << hostname << "/exec-wholan/connected_hosts-" << hostname << " interval=" << interval << "N:" << n_connected << std::endl;
		usleep(interval * 1000000);
	}
	return 0;
}