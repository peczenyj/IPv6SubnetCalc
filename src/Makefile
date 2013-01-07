CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		IPv6SubnetCalc.o main.o

LIBS =

TARGET =	../bin/IPv6SubnetCalc

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	
test:	$(TARGET)
	@../test/test.sh 
	
