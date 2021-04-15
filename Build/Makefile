GCC = gcc
GPP = g++
GCCFLAGS = -std=c11
GPPFLAGS = -std=c++17
SERV_NAME = ../server
CLIENT_NAME = ../client
SERVDIR = ../src/client_and_server
AVLDIR = ../src/avl

all: serv.o avl.o avl_wrapper.o client.o ui.o zmq_handle.o
	$(GCC) $(GCCFLAGS) zmq_handle.o client.o -o $(CLIENT_NAME) -lzmq 
	$(GPP) $(GPPFLAGS) zmq_handle.o serv.o avl.o avl_wrapper.o ui.o -o $(SERV_NAME) -lzmq 

client.o: $(SERVDIR)/client.c
	$(GCC) $(GCCFLAGS) -c $(SERVDIR)/client.c -o client.o

serv.o: $(SERVDIR)/server.c $(SERVDIR)/ui.h
	$(GCC) $(GCCFLAGS) -c $(SERVDIR)/server.c -o serv.o

zmq_handle.o: $(SERVDIR)/zmq_handle.c $(SERVDIR)/zmq_handle.h
	$(GCC) $(GCCFLAGS) -c $(SERVDIR)/zmq_handle.c -o zmq_handle.o

ui.o: $(SERVDIR)/ui.c $(SERVDIR)/ui.h
	$(GCC) $(GCCFLAGS) -c $(SERVDIR)/ui.c -o ui.o

avl_wrapper.o: $(AVLDIR)/avl_wrapper.cpp $(AVLDIR)/avl_wrapper.hpp
	$(GPP) $(GPPFLAGS) -c $(AVLDIR)/avl_wrapper.cpp -o avl_wrapper.o

avl.o: $(AVLDIR)/avl.cpp $(AVLDIR)/avl.hpp
	$(GPP) $(GPPFLAGS) -c $(AVLDIR)/avl.cpp -o avl.o

clean:
	rm *.o $(SERV_NAME) $(CLIENT_NAME)
