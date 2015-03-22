#ifndef _BNB_SERVER_
#define _BNB_SERVER_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <util/common/bnblog.hpp>
#include <ei/external/bnbprotocol.hpp>
#include "bnbchild.hpp"

static const char* wrongUsage = 
"Wrong Usage. Use: bnbsolver.exe port_number";

/**
 * Maximal length of pending requests
 */
static const int maxq = 3;


/**
 * Run and communicate BNB-Solver remotely
 */

class BNBServer{
 public:

  /**
   * Extracts parameters from the command-line arguments
   */
  void parsArgs(int argc, char* argv[])
    {
      if(argc == 3) {
	mPort = atoi(argv[1]);
        mChildPort = atoi(argv[2]);
      } else {
	Error(wrongUsage);
      }      
    }

  void mainLoop()
    {
      setupSocket();      
    }

 private:
  /**
   * Sets up socket and start listening
   */
  void setupSocket()
    {
      int pn = mChildPort;
      mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
      if(mSock < 0)
	Error("socket creation failed");
      struct sockaddr_in bnbsolverSA;
      bnbsolverSA.sin_family = AF_INET;
      bnbsolverSA.sin_port = htons(mPort);
      bnbsolverSA.sin_addr.s_addr = INADDR_ANY;
      if(bind(mSock, (struct sockaddr*) &bnbsolverSA, sizeof(bnbsolverSA)) < 0)
	Error("bind failed");
      if(listen(mSock, maxq) < 0)
	Error("listen failed");

      printf("%s\n", BNB_SERVER_STARTED_KEYWORD);
      fflush(stdout);
      while(1) {
	int newsd;

	newsd = accept(mSock, NULL, NULL);
	if(newsd < 0)
	  Error("accept failed");
	else {
	  int rc;
          pn ++;
	  rc = fork();
	  if(rc == 0) {
	    close(mSock);
	    BNBChild bc (newsd, pn);
	    
	    bc.run();
	    close(newsd);
	    exit(0);
	  } else {
	    mLogger << "BNBChild " << rc << " forked\n";
	    mLogger.write();
	    close(newsd);
	  }
	}	  
      }
    }

  /**
   * Handle an error
   */
  void Error(const char* text) const 
    {
      fprintf(stderr, "ERROR: %s\n", text);
      fflush(stderr);
      exit(-1);
    }

  /**
   * Communicating port
   */
  int mPort;
  
  int mChildPort;

  /**
   * Listening socket
   */
  int mSock;

  /**
   * Logger
   */
  BNBLog<BNB_DO_LOGGING> mLogger;
};

#endif
