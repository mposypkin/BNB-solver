/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbedebug.cpp
 *
 * Utility for connecting to BNBSolver via BNB-External-Interface.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stack>
#include <time.h>
#include <util/common/util.hpp>
#include <ei/external/bnbcommand.hpp>
#include <ei/external/bnbcmdtags.hpp>
#include <kernel/bnblib/bnb.hpp>


#define CMD_LOAD "<load>"
#define CMD_QUIT "<quit>"
#define CMD_SKIP "<skip>"

static void Error(const char* str)
{
    fprintf(stderr, "ERROR: %s\n", str);
    fflush(stderr);
    exit(-1);
}



main(int argc, char* argv[])
{
  int s, t ,r;
  const int len = 256;
  int port;
  char name[len];
  char ipaddr[len];
  char buf[len];
  socklen_t salen = sizeof(struct sockaddr);
  struct hostent* he;
  struct in_addr saddr;
  struct sockaddr_in sa;
  struct sockaddr sa1;
  std::stack <FILE*> sf;
  time_t startTime;  

  if(argc != 2)
    Error("Usage: program port_number\n");
  else
    port = atoi(argv[1]);

  gethostname(name, len);
  printf("host name is [%s]\n", name);
  he = gethostbyname(name);

  if(he == NULL) 
    Error("gethostbyname failed");
  else {
    char** addr;
    addr = he->h_addr_list;
		
    while (addr[0] != NULL){		
      sprintf(ipaddr, "%d.%d.%d.%d",
	      (int) addr[0][0],
	      (int) addr[0][1],
	      (int) addr[0][2],
	      (int) addr[0][3]);
      printf("host [%s] detected\n", ipaddr);
      addr ++;
    }
  }
  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	    
  printf("socket opened: %d\n", s);
  
  inet_aton(ipaddr, &saddr);
	    
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);
  sa.sin_addr = saddr;

  if(bind(s, (struct sockaddr*) &sa, sizeof(sa)) == 0)
    printf("bound!\n");
  else
    Error("bind failed");
	    
  if(listen(s, 1) == 0)
    printf("listening ...\n");
  else
    Error("listen falied");
  
  t = accept(s, &sa1, &salen);
	    
  if(t != -1) 
    printf("accept returned: %d\n", t);
  else
    Error("accept failed\n");


  sf.push(stdin);
  startTime = time(NULL);
  while(1) {
    struct timeval tv;
    fd_set st;

    tv.tv_sec = 0;
    tv.tv_usec = 10;
    FD_ZERO(&st);
    FD_SET(fileno(sf.top()), &st);
    FD_SET(t, &st);  

    printf("%d:=>", time(NULL) - startTime);    
    fflush(stdout);

    if(select(BNBMAX(fileno(sf.top()),t) + 1, &st, NULL, NULL, NULL)) {
	const int cmd_start = 0;
	const int cmd_read = 1;
	int mode = cmd_start;
	std::string cmd;
	int c;
	if(FD_ISSET(fileno(sf.top()), &st)) {
	  cmd = "";
	  while(1) {      
	    if(sf.empty())
	      break;
	  
	    c = fgetc(sf.top());
	    if(c == EOF){
	      sf.pop();
	      break;
	    }	
	    if((mode == cmd_start) && (c == BNB_COMMAND_START)) {
	      cmd += c;
	      mode = cmd_read;
	    }
            else if(c == BNB_COMMAND_END) {
	      cmd += c;
	      break;
	    }
	    else if(mode == cmd_read)
	      cmd += c;
	  }
	  if(cmd == CMD_QUIT) 
	    break;
	  else if(cmd == CMD_LOAD) {
	    FILE* f;
	    char s[1024];
	  
	    printf("Enter file name: ");
	    fscanf(sf.top(), "%s", s);
	    f = fopen(s, "r");
	    if(f == NULL)
	      printf("Failed to open file\n");
	    else
	      sf.push(f);
	    continue;
	  } else if(cmd == CMD_SKIP) {
	  } else {
	    send(t, cmd.c_str(), cmd.size(), 0);
	    printf("%d:sent: %s\n", time(NULL) - startTime, cmd.c_str());
	  }	
	} else {
	    cmd = "";
	    mode = cmd_start;
	    while(1) {     
	      r = recv(t, buf, 1, 0);
	      if(r != 1){
		printf("Solver disconnected\n");	 
		close(t);
		close(s);
		exit(0);
	      } else if((mode == cmd_start) && (((int)buf[0]) == BNB_COMMAND_START)) {
		cmd += buf[0];
		mode = cmd_read;
              } else if(((int)buf[0]) == BNB_COMMAND_END) {
		cmd += buf[0];
		break;
	      } else if(mode == cmd_read)
	      cmd += buf[0];
	    }    
	  printf("%d:solver reply: %s\n", time(NULL) - startTime, cmd.c_str());   
	  fflush(stdout);

	}
    }
  }
  close(t);
  close(s);
}
