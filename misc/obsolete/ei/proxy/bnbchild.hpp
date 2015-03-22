#ifndef __BNB_CHILD__
#define __BNB_CHILD__

#include <unistd.h>
#include <netdb.h>
#include <deque>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <util/common/bnblog.hpp>
#include <util/common/bnberrcheck.hpp>
#include <ei/external/bnbcommand.hpp>
#include <ei/external/bnbcmdparser.hpp>
#include <ei/external/bnbprotocol.hpp>
#include <ei/external/bnbdefs.hpp>
#include <util/xml/xmlpars.hpp>

/**
 * BNB Child process
 */
class BNBChild {
  private:
  enum GlobalState {
    Start,
    RunSolver,
    WaitConn,
    DuplexConn
  };
  
  enum CommandSource {
    FromAgent,
    FromSolver
  };

public:

  /**
   * The constructor 
   * @param sd socket descriptor
   * @param portn solver port number
   */
  BNBChild(int sd, int portn)
  {
    mAgentSock = sd;
    mSolverPort = portn;
    mRunScriptName = "runsolver.sh";
    mLogger.setName("BNB Child");
    mLogger.setFile("/tmp/child.log");
    mState = Start;
  }
  
  void sendCommandToAgent(BNBCommand& cmd)
  {
    std::string s;
    BNBCommandParser::toString(cmd, s);
    send(mAgentSock, s.c_str(), s.size(), 0);
  }
  
  /**
   * Read a command from char buffer
   * 
   * @param buf buffer of chars
   * @param len the length of the buffer
   * @param from from what source
   * @return true if a number of command was read, false if the reading is not completed
   */
  void readCommand(char* buf, int len, int from)
  {
    enum State {
      opened,
      closed
    };
    int is, ie, i;
    static State state = closed;
    std::string *strbuf;
    static std::string agentBuf;
    static std::string solverBuf;
    is = 0;
    if(from == FromAgent)
      strbuf = &agentBuf;
    else 
      strbuf = &solverBuf;
    
    for(i = 0; i < len; i ++) {
      if(buf[i] == BNB_COMMAND_START) {
        if(state == closed) {
          is = i;
          state = opened;
        } else 
          BNB_ERROR_REPORT("syntax error");        
      } else if(buf[i] == BNB_COMMAND_END) {
        if(state == opened) {
          strbuf->append(buf + is, i - is + 1);
          BNBCommand cmd;
          mLogger << "Processing command: " << *strbuf << "\n";
          mLogger.write();
          BNBCommandParser::fromString(*strbuf, cmd);
          if(from == FromAgent)
            mFromAgent.push_front(cmd);
          else 
            mFromSolver.push_front(cmd);
          mLogger << "Command " << BNBCommandParser::toString(cmd) << " added to the queue\n";
          mLogger << "len = " << len << "," << "i = " << i << "\n"; 
          mLogger.write();
          strbuf->clear();
          state = closed;
        } else
          BNB_ERROR_REPORT("syntax error");
      } else if(i == (len - 1)) {
        if(state == opened) {
          strbuf->append(buf + is, i - is + 1);
          mLogger << "The command " << *strbuf << " upgraded\n";
          mLogger.write();
        } 
      }        
    }    
  }

  /**
   * Runs solver
   * @param cmd running command
   */
  void runSolver(BNBCommand & cmd)
  {
    int np, rc, maxtime;
    std::ostringstream os;
    XMLPars pars;
    std::map<std::string, std::string> tls;
    std::string oname, odata;
    pars.parse(cmd.mDsc, tls);
    maxtime = -1;
    if(tls.find(RUNSOLVER_NP_TAG) == tls.end()){
      BNB_ERROR_REPORT("Number of processors not set");
    } else {
      np = atoi(tls[RUNSOLVER_NP_TAG].c_str());
    }
    if(tls.find(RUNSOLVER_MAXTIME_TAG) != tls.end()){
      maxtime = atoi(tls[RUNSOLVER_MAXTIME_TAG].c_str());
    }
    mLogger << "Spawning " << np << " process solver\n";
    mLogger.write();    
    os << mRunScriptName << " " << np << " " << getip() << " " << mSolverPort;
    if(maxtime != -1) {
      os << " " << maxtime;
    }
    rc = system(os.str().c_str());
    mLogger << "return code = " << rc << "\n";
    mLogger.write();
  }
  
  
  void setupSolverSocket()
  {
    mSolverPort ++;
    printf("solver port %d\n", mSolverPort);
    fflush(stdout);
    mSolverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(mSolverSock < 0)
      BNB_ERROR_REPORT("socket creation failed");
    struct sockaddr_in bnbsolverSA;
    bnbsolverSA.sin_family = AF_INET;
    bnbsolverSA.sin_port = htons(mSolverPort);
    bnbsolverSA.sin_addr.s_addr = INADDR_ANY;
    if(bind(mSolverSock, (struct sockaddr*) &bnbsolverSA, sizeof(bnbsolverSA)) < 0)
      BNB_ERROR_REPORT("bind failed");
    if(listen(mSolverSock, 1) < 0)
      BNB_ERROR_REPORT("listen failed");    
  }
  
  /**
   * Set up a socket and run solver
   * @param cmd running command
   */
  void spawnSolver(BNBCommand & cmd)
  {
    setupSolverSocket();
    runSolver(cmd);
  }
    
  /**
   * Processes non-empty command queue retrieved from agents
   */
  void processAgentCommand()
  {
    while(!mFromAgent.empty()) {
      BNBCommand cmd;
      cmd = mFromAgent.back();
      mFromAgent.pop_back();
      mLogger << "Processing command from Agent: " << BNBCommandParser::toString(cmd) << "\n";
      mLogger.write();
      if(cmd.mName == BNB_RUN) {
        if(mState == Start)
          spawnSolver(cmd);
        else
          BNB_ERROR_REPORT("Trying to run multiple solvers");
        mState = WaitConn;
      }
    }
  }
  
  /**
   * Runs the child
   */
  void run()
  {
    mState = Start;
    mLogger << "is running\n";
    mLogger.write();      
    for(;;) {
      const int len = 1024;
      char buf[len];
      int cnt, rc, maxs;
      fd_set rmask;
      FD_ZERO(&rmask);
      if(mState == WaitConn) {
        FD_SET(mAgentSock, &rmask);
        FD_SET(mSolverSock, &rmask);
        maxs = BNBMAX(mAgentSock, mSolverSock) + 1;
      } else if(mState == DuplexConn) {
        FD_SET(mAgentSock, &rmask);
        FD_SET(mSolverDataSock, &rmask);
        maxs = BNBMAX(mAgentSock, mSolverDataSock) + 1;
      } else {
        FD_SET(mAgentSock, &rmask);
        maxs = mAgentSock + 1;
      }
      rc = select(maxs, &rmask, NULL, NULL, NULL);    
      if(rc == -1)
        BNB_ERROR_REPORT("Select failed");
      if(FD_ISSET(mAgentSock, &rmask)) {
        cnt = read(mAgentSock, buf, len);
        if(cnt == 0) {
          mLogger << "connection broken from agent\n";
          mLogger.write();
          break;
        } else if(cnt == -1) {
          BNB_ERROR_REPORT("BNB Child failed to read from socket");
        } else {
          mLogger << "received " << std::string(buf, cnt) << "\n";
          mLogger.write();
          if(mState == DuplexConn) {
           rc = send(mSolverDataSock, buf, cnt, 0);
           if(rc == -1)
             BNB_ERROR_REPORT("Send failed"); 
          } else  {
            readCommand(buf, cnt, FromAgent);
            if(!mFromAgent.empty())
              processAgentCommand();
          }
        }
      } else if ((mState == WaitConn) && (FD_ISSET(mSolverSock, &rmask))) {
        mSolverDataSock = accept(mSolverSock, NULL, 0);
        if(mSolverDataSock == -1)
            BNB_ERROR_REPORT("Accept failed");
        mState = DuplexConn;
        mLogger << "Connection from solver accepted\n";
        mLogger.write();
      } else if((mState == DuplexConn) && FD_ISSET(mSolverDataSock, &rmask)) {
        cnt = read(mSolverDataSock, buf, len);
        if(cnt == 0) {
          BNBCommand cmd;
          std::string s;
          mLogger << "connection broken from solver\n";
          mLogger.write();
          /*
          mState = Start;
          cmd.mName = BNB_BRK_CONN;
          s = BNBCommandParser::toString(cmd);
          rc = send(mAgentSock, s.c_str(), s.size(), 0);
          if(rc == -1) 
            BNB_ERROR_REPORT("Send failed");
          continue;
          */
          break;
        } else if(cnt == -1) {
          BNB_ERROR_REPORT("BNB Child failed to read from socket");
        } else {
          mLogger << "received " << std::string(buf, cnt) << " from solver. Sending to agent ...\n";
          mLogger.write();
          rc = send(mAgentSock, buf, cnt, 0);
          if(rc == -1) 
            BNB_ERROR_REPORT("Send failed");
        }     
      }
    }
    close(mAgentSock);
    close(mSolverDataSock);
    mLogger << "terminated\n";
    mLogger.write();
  }
  
private:
  
  /**
   * Retrieve host ip
   * @return ip address
   */
  std::string getip()
  {
    const int len = 256;
    char name[len], ipaddr[len];
    struct hostent* he;
    
    gethostname(name, len);
    he = gethostbyname(name);
    BNB_ASSERT(he);
    char** addr;
    addr = he->h_addr_list;    
    while (addr[0] != NULL){		
      sprintf(ipaddr, "%d.%d.%d.%d",
              (int)(unsigned char) addr[0][0],
               (int)(unsigned char) addr[0][1],
                (int)(unsigned char) addr[0][2],
                 (int)(unsigned char) addr[0][3]);
      addr ++;
    }
    return ipaddr;
  }
    
  int mAgentSock;
  
  std::deque<BNBCommand> mFromAgent;

  std::deque<BNBCommand> mFromSolver;
  
  BNBLog < BNB_DO_LOGGING > mLogger;

  int mState;
  
  int mSolverPort;
  
  int mSolverSock;
  
  int mSolverDataSock;
  
  std::string mRunScriptName;
};

#endif
