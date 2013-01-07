//============================================================================
// Name        : IPv6SubnetCalc.cpp
// Author      : Tiago Peczenyj
// Version     : 0.1
// Copyright   : BSD License
// Description : Small POC of IPv6 subnet calculator
//============================================================================

#include <list>
#include <iostream>

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "IPv6SubnetCalc.h"

/**
 *  act as a constructor to build one IPv6 from an anddress and one prefix
 *  returns one IPv6 and throw exception (char *) in case of one error (like not valid ip address)
 */ 

IPv6 createIPv6fromString(std::string str, int prefix=128) {
	IPv6 ip;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof hints);

	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags    = AI_PASSIVE;

	status = getaddrinfo(str.c_str(),"echo",&hints,&servinfo);

	if(status !=0) {
		throw gai_strerror(status);
	} else if(servinfo->ai_family != AF_INET6){
		throw "Not valid ipv6 address!";
	}

	struct in6_addr addr = ((struct sockaddr_in6 *) servinfo->ai_addr)->sin6_addr;

	freeaddrinfo(servinfo);

	ip.prefix = prefix;
	ip.addr = addr;

	return ip;
}

/**
 *  Create a readable representation for one IPv6
 */ 

std::string IPv6toCompressedString(IPv6 ip){
	char ipstr[INET6_ADDRSTRLEN];
	inet_ntop(AF_INET6, &(ip.addr), ipstr, sizeof ipstr);
	return std::string(ipstr);
}

/**
 *  Create a complete representation for one IPv6
 */ 

std::string IPv6toString(IPv6 ip){
	char str[128];

	sprintf(str, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
	                 (int)ip.addr.s6_addr[0], (int)ip.addr.s6_addr[1],
	                 (int)ip.addr.s6_addr[2], (int)ip.addr.s6_addr[3],
	                 (int)ip.addr.s6_addr[4], (int)ip.addr.s6_addr[5],
	                 (int)ip.addr.s6_addr[6], (int)ip.addr.s6_addr[7],
	                 (int)ip.addr.s6_addr[8], (int)ip.addr.s6_addr[9],
	                 (int)ip.addr.s6_addr[10], (int)ip.addr.s6_addr[11],
	                 (int)ip.addr.s6_addr[12], (int)ip.addr.s6_addr[13],
	                 (int)ip.addr.s6_addr[14], (int)ip.addr.s6_addr[15]);

	return std::string(str);
}

/**
 *  Create one "mask" for the IPv6 based on the prefix
 */ 

IPv6 addressFromIp(IPv6 ip){
	IPv6 address;
	int imask = 128 - ip.prefix;
	for(int i=15;i>=0;i--){
		int j = (imask > 8)? 8 :imask;
		unsigned char x = (1 << j) -1;

		address.addr.s6_addr[i] = ~x;
		imask -= j;
	}

	return address;
}

 /**
  *  Creates a subnet from one ip address and one mask length
  */ 

IPv6Subnet createIPv6Subnet(IPv6 address, int mask) {
	if(mask > 128 || mask < 0){
		throw "Incorrect ipv6 prefix, must be between 1 to 128";
	}

	IPv6Subnet subnet;
	subnet.mask = mask;
	memset(&subnet.begin, 0, sizeof(IPv6));
	memset(&subnet.end,   0, sizeof(IPv6));


	/* calculus of begin and end of subnet based on mask
	 * the subnet fec0:bebe:cafe::/122 is between fec0:bebe:cafe:: and fec0:bebe:cafe::3f
	   0x00 to 0x3f -> 0 to 63 -> 64 possible addresses. or 2^6
	   one ipv6 has 128 bits and in this example we use 122 to the subnet. 128-122 is 6 bits.

	   begin 00000000 -> 0
	   end   00111111 -> 63

	   to find the begin we calculate the complement:
	   ~00111111 -> 11000000

	   and apply the 'and' bit-to-bit just reset the first 6 bits in this example.

	   to find the end, we just apply one 'or' bit-to-bit with the mask
	 */
	int imask = 128 - mask;
	for(int i=15;i>=0;i--){
		int j = (imask > 8)? 8 :imask;
		unsigned char x = (1 << j) -1;

		subnet.begin.addr.s6_addr[i] = address.addr.s6_addr[i] & ~x;
		subnet.end.addr.s6_addr[i]   = address.addr.s6_addr[i] |  x;

		imask -= j;
	}

	subnet.begin.prefix = mask;
	subnet.end.prefix = mask;
	return subnet;
}

 /**
  *  Just a helper to create one subnet based on the prefix of one ipv6 address
  */ 
IPv6Subnet createIPv6Subnet(IPv6 address){
	return createIPv6Subnet(address, address.prefix);
}

 /**
  *  Calculate the next ip address (the successor)
  *  Important to split subnets!
  */ 
IPv6 succ(IPv6 old){
	IPv6 ip = old;
	for(int i=15;i>=0;i--){
		if(ip.addr.s6_addr[i] != 0xff){
			ip.addr.s6_addr[i] += 1;
			break;
		}
		ip.addr.s6_addr[i] = 0x00;
	}
	return ip;
}

 /**
  *  Split one subnet in many based on bitmask lenght
  *  
  *  Example: Split the subnet fec0:bebe:cafe::/64 in many subnets with 65 bits
  * 
  *  We can split in two subnets -> 2**(65 - 64)
   
  [fec0:bebe:cafe::/64]

	[IPV6 INFO]
	 Expanded Address  - fec0:bebe:cafe:0000:0000:0000:0000:0000
	 Compressed address- fec0:bebe:cafe::
	 Prefix address    - ffff:ffff:ffff:ffff:0000:0000:0000:0000
	 Prefix length     - 64
	 Network range     - fec0:bebe:cafe:0000:0000:0000:0000:0000
	                     fec0:bebe:cafe:0000:ffff:ffff:ffff:ffff
	
	[Split network]
	 Split the current network into subnets of 65 bits of size
	 Found 2 subnet(s) 
	
	 Network range     - fec0:bebe:cafe:0000:0000:0000:0000:0000
	                     fec0:bebe:cafe:0000:7fff:ffff:ffff:ffff
	 Network range     - fec0:bebe:cafe:0000:8000:0000:0000:0000
	                     fec0:bebe:cafe:0000:ffff:ffff:ffff:ffff
 
  */ 
std::list<IPv6Subnet> IPv6SubnetSplit(IPv6Subnet subnet, int mask){
	std::list<IPv6Subnet> subnets;

	if(mask > 128 || mask < 1){
		throw "Incorrect subnet mask, must be between 1 to 128";
	} else if(subnet.mask > mask){
		throw "Subnet Mask cannot be bigger than Split mask";
	} else {
		int num_subnets = 1 << (mask - subnet.mask);

		IPv6Subnet sub = createIPv6Subnet(subnet.begin,mask);
		subnets.push_back(sub);

		for(int i=1;i<num_subnets;i++){
			sub = createIPv6Subnet(succ(sub.end),mask);
			subnets.push_back(sub);
		}
	}

	return subnets;
}
