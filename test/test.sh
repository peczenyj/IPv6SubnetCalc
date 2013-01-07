#!/bin/bash
#
# Acceptance test for ipv6 subnet calculator
# All test cases has one master file
# Verify the test is based on diff outputs
# Exit - number of failures
# Tiago Peczenyj

ALL=0
PASS=0
FAIL=0

BASEDIR=$(dirname $0)
MASTERDIR=${BASEDIR}/master
DIFFDIR=${BASEDIR}/diff

echo "DEBUG: clean all diffs files" && rm -v ${DIFFDIR}/*.diff 2>&- 

function verify(){
	let ALL++
	local TEST_NAME=$1
	if diff /tmp/${TEST_NAME}.log ${MASTERDIR}/${TEST_NAME}.log  > ${DIFFDIR}/${TEST_NAME}.diff 
	then
		let PASS++
		echo "$TEST_NAME ..OK"
		rm ${DIFFDIR}/${TEST_NAME}.diff
	else
		let FAIL++
		echo "$TEST_NAME ..NOK (see ${DIFFDIR}/${TEST_NAME}.diff)"
	fi
}

IPCALC=../bin/IPv6SubnetCalc

echo "TEST usage message"
$IPCALC                          >  /tmp/usage.log 2>&1 # the usage message
verify usage

echo "TEST basic ips"
$IPCALC ::                       >  /tmp/basic.log 2>&1 # full-00 address
$IPCALC ::1                      >> /tmp/basic.log 2>&1 # ::0001 address
$IPCALC fec0:bebe:cafe::         >> /tmp/basic.log 2>&1 # just one example
$IPCALC fec0:bebe:cafe::1/88     >> /tmp/basic.log 2>&1 # example with prefix
verify basic

echo "TEST subnets" 
$IPCALC ::1/11  12               > /tmp/subnet.log 2>&1 # test format
$IPCALC ::127.0.0.1              >>/tmp/subnet.log 2>&1 # test ipv4 format
$IPCALC fec0:bebe:cafe::/64 65   >>/tmp/subnet.log 2>&1 # test   2 subnets
$IPCALC fec0:bebe:cafe::/122 124 >>/tmp/subnet.log 2>&1 # test   4 subnets
$IPCALC fec0:bebe:cafe::/120 128 >>/tmp/subnet.log 2>&1 # test 256 subnets
verify subnet

echo "TEST error messages"
$IPCALC lol                      2>  /tmp/error.log >&- # not ip
$IPCALC 127.0.0.1                2>> /tmp/error.log >&- # not ipv6
$IPCALC ::1/-1                   2>> /tmp/error.log >&- # invalid prefix
$IPCALC ::1/129                  2>> /tmp/error.log >&- # ''
$IPCALC ::1 -1                   2>> /tmp/error.log >&- # invalid bits for subnet splitting
$IPCALC ::1 129                  2>> /tmp/error.log >&- # ''
$IPCALC fec0:bebe:cafe::/65 64   2>> /tmp/error.log >&- # less bits to splitting than prefix
verify error

echo ""
echo "TEST DONE, result:"
echo "ALL ($ALL) PASS($PASS)/FAIL($FAIL)"

exit $FAIL