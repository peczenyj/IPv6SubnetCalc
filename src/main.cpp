/*
 * main.cpp
 *
 *  Created on: 07/01/2013
 *      Author: tiago peczenyj
 */

#include <list>
#include <iostream>

#include "IPv6SubnetCalc.h"

void showUsage();
void showSubnet(IPv6, IPv6Subnet);
void showSplitSubnets(std::list<IPv6Subnet>, int);

void print(std::string::size_type n, std::string const &s)
{
    if (n == std::string::npos) {
        std::cout << "not found\n";
    } else {
        std::cout << "found: " << s.substr(n) << '\n';
    }
}

int main (int argc, char **argv){
	int mask = 128;

	if(argc < 2){
		showUsage();
	} else if(argc >= 2){
		std::string::size_type n;
	    std::string address = argv[1];

	    if ((n = address.find("/")) != std::string::npos) {
	    	mask = std::atoi(address.substr(n+1).c_str());
	    	address = address.substr(0,n);
	    }

	    try {
			IPv6 ip = createIPv6fromString(address, mask);
			IPv6Subnet subnet = createIPv6Subnet(ip);
			showSubnet(ip, subnet);

			if(argc > 2){
				int bits = std::atoi(argv[2]);
				std::list<IPv6Subnet> subnets = IPv6SubnetSplit(subnet, bits);
				showSplitSubnets(subnets, bits);
			}
	    }  catch (std::exception& e) {
	    	std::cerr << "Exception : " << e.what() << std::endl;
	    	return EXIT_FAILURE;
	    } catch (const char* message ){
	    	std::cerr << "Error: " << message << std::endl;
	    	return EXIT_FAILURE;
	    }
	}

	return EXIT_SUCCESS;
}


void showUsage(){
	std::cout << "\tUsage: "                            << std::endl;
	std::cout << "\tIPv6SubnetCalc ip"                  << std::endl;
	std::cout << "\t - show information about the ip "  << std::endl;
	std::cout << std::endl;

	std::cout << "\tIPv6SubnetCalc ip bits"             << std::endl;
	std::cout << "\t - split subnets based on bits"     << std::endl ;
	std::cout << std::endl;

	std::cout << "\tExamples: "                         << std::endl;
	std::cout << "\tIPv6SubnetCalc fec0:bebe:cafe::"    << std::endl;
	std::cout << "\t - consider 128 bits of prefix "    << std::endl;
	std::cout << std::endl;

	std::cout << "\tIPv6SubnetCalc fec0:bebe:cafe::/64" << std::endl;
	std::cout << "\t - consider 64 bits of prefix "     << std::endl;
	std::cout << std::endl;

	std::cout << "\tIPv6SubnetCalc fec0:bebe:cafe::/64 65"    << std::endl;
	std::cout << "\t - show 65 bits subnets in this address"  << std::endl;
	std::cout << std::endl;
}

void showSubnet(IPv6 ip, IPv6Subnet subnet){
	std::cout << "[" << IPv6toCompressedString(ip) << "/" << ip.prefix << "]" << std::endl << std::endl;

	std::cout << "[IPV6 INFO]" << std::endl;

	std::cout << " Expanded Address  - " << IPv6toString(ip) << std::endl;
	std::cout << " Compressed address- " << IPv6toCompressedString(ip) << std::endl;
	std::cout << " Prefix address    - " << IPv6toString(addressFromIp(ip)) << std::endl;
	std::cout << " Prefix length     - " << ip.prefix << std::endl;
	std::cout << " Network range     - " << IPv6toString(subnet.begin) << std::endl;
	std::cout << "                     " << IPv6toString(subnet.end)   << std::endl << std::endl;
}

void showSplitSubnets(std::list<IPv6Subnet> subnets, int bits){
	std::cout << "[Split network]" << std::endl;

	std::cout << " Split the current network into subnets of " << bits << " bits of size" << std::endl;
	std::cout << " Found " << subnets.size() << " subnet(s) "<< std::endl << std::endl;

	for (std::list<IPv6Subnet>::iterator it=subnets.begin(); it != subnets.end(); ++it){
		std::cout <<" Network range     - " << IPv6toString(it->begin) << std::endl;
		std::cout <<"                     " << IPv6toString(it->end) << std::endl;
	}
}
