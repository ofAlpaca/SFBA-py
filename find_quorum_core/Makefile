CFLAGS = -Wall -O2 -std=c++11

.PHONY: all findSCC clean

all: findSCC 
	python findQuorumCore.py

findSCC:
	g++ -o SFBA.so findSCC.cpp -std=c++11 -fPIC -shared \
-Wall -Wextra `python2.7-config --includes --libs` \
-lboost_python

clean:
	rm -rf SFBA.so *.png
