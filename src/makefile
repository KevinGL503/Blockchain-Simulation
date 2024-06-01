#makefile
output: Driver.o Miner.o EventsQueue.o Blockchain.o
	g++ Driver.o Miner.o EventsQueue.o Blockchain.o sha256.o -o ../output
Driver.o: Driver.cpp
	g++ -c Driver.cpp
Miner.o: Miner.cpp
	g++ -c Miner.cpp
Blockchain.o: Blockchain.cpp sha256.o
	g++ -c Blockchain.cpp
sha256.o: sha256.cpp sha256.h
	g++ -c  sha256.cpp
EventsQueue.o: EventsQueue.cpp EventsQueue.h
	g++ -c  EventsQueue.cpp
