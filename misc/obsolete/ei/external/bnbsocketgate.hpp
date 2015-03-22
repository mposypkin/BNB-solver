#ifndef _BNBSOCKETGATE_HPP_
#define _BNBSOCKETGATE_HPP_
/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * Implementation of the BNBGate inerface for sockets
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>
#include <unistd.h>

#include "bnbchargate.hpp"
#include <util/common/bnberrcheck.hpp>



/**
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 *
 * File gate exception to throw
 */
class BNBSocketGateError {
public:  
/**
 * Constructor
 *
 * @param errorDescr
 *
 */
  BNBSocketGateError(const char* errorDescr)
  {
    fprintf(stderr, "%s\n", errorDescr);
    fflush(stderr);
    abort();
  }
};

/**
 *
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * 
 * Class implements BNBGate interface to sockets
 */
class BNBSocketGate : public BNBCharGate{
public:

/**
 * Constructor
 *
 * @param ip ip address of an agent
 *
 * @param port of an agent
 *
 */
  BNBSocketGate(std::string ip, std::string port)
  {
    int rc;

    mSock = -1;
    mAddr.sin_family = AF_INET;
    printf("%s\n", ip.c_str());
    fflush(stdout);
    rc = inet_aton(ip.c_str(), &(mAddr.sin_addr));
    if(rc == 0)
      throw BNBSocketGateError("Bad address\n");
    mAddr.sin_port =  htons(atoi(port.c_str()));  
  }

/**
 * Open connection to an gent
 *
 */
  void open()
  {
    int rc;

    mSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mSock == -1)
      BNBSocketGateError("Socket open failed");
    
    rc = connect(mSock, (struct sockaddr*) &mAddr, sizeof(mAddr));
    if(rc == -1)
      BNBSocketGateError("Connect failed");

  }
  
/**
 * Close gate
 *
 */
  void close()
  {
    if(mSock != -1)
      ::close(mSock);
  }


  /**
   * Probes whether data is ready to receive
   */
  bool probe()
  {
#ifndef __USE_POLL__
    int rc;
    struct timeval tv;
    fd_set rst;
    fd_set est;

    tv.tv_sec = 0;
    tv.tv_usec = 10;
    FD_ZERO(&rst);
    FD_SET(mSock, &rst);  
    FD_ZERO(&est);
    FD_SET(mSock, &est);  

    rc = select(mSock + 1, &rst, NULL, &est, &tv);
    if(rc == -1){
      BNB_ERROR_REPORT("select falied");
    } else if(FD_ISSET(mSock, &est))
      BNB_ERROR_REPORT("pending socket error in select");
    return (rc != 0);
#else
    int rc;
    struct pollfd fds[1];
    fds[0].fd = mSock;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    rc = poll(fds, 1, 100);
    if(rc == -1)
      BNB_ERROR_REPORT("poll failed");
    return rc > 0;
#endif
  }


private:

  
/**
 * Reads the input symbol
 *
 * @return read symbol
 *
 */
  int readc()
  {
    unsigned char c;
    int rc;

    rc = recv(mSock, &c, 1, 0);
    if(rc != 1)
      throw BNBSocketGateError("Socket error on read");
    fflush(stdout);
    return (int)c;
  }

/**
 * Writes byte seq the output buffer
 *
 * @param str byte seq
 *
 * @param sz number of bytes
 *
 */
  void writes(const char* str, int sz)
  {
    int rc;

    rc = send(mSock, str, sz, 0);
    if(rc != sz)
      throw BNBSocketGateError("Socket error on write");      
  }


/**
 * Agent INET address
 */
    
  struct sockaddr_in mAddr;

/**
 * Socket
 */
  int mSock;

};

#endif
