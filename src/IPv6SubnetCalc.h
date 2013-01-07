/*
 * IPv6SubnetCalc.h
 *
 *  Created on: 07/01/2013
 *      Author: tiago peczenyj
 */

#ifndef IPV6SUBNETCALC_H_
#define IPV6SUBNETCALC_H_

#include <netdb.h>

typedef struct {
	int prefix;
	struct in6_addr addr;
} IPv6;

typedef struct {
	int mask;
	IPv6 begin, end;
} IPv6Subnet;


IPv6 createIPv6fromString(std::string, int);

std::string IPv6toCompressedString(IPv6 ip);

std::string IPv6toString(IPv6);

IPv6 addressFromIp(IPv6);

IPv6 succ(IPv6);

IPv6Subnet createIPv6Subnet(IPv6, int);

IPv6Subnet createIPv6Subnet(IPv6);

std::list<IPv6Subnet> IPv6SubnetSplit(IPv6Subnet, int);

#endif /* IPV6SUBNETCALC_H_ */
