#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <util/common/util.hpp>
#include <ei/external/bnbfilegate.hpp>
#include <ei/external/bnbsocketgate.hpp>
#include <ei/external/bnbcmdparser.hpp>
#include <util/xml/xmlpars.hpp>
#include <util/common/fileutils.hpp>
#include <ei/external/bnbdefs.hpp>


void submitWork(std::vector<BNBCommand>& cmdv) {
  BNBCommand cmd;
  cmd.mName = "DUMMY";
  cmd.mCode = "set $ONFINISH exit;";
  cmdv.insert(cmdv.end() - 1, cmd);
//  cmdv.push_back(cmd);
  FILE* f = fopen(BNB_IN_FILE, "w");
  std::vector<BNBCommand>::const_iterator i;
  for(i = cmdv.begin(); i != cmdv.end(); i ++) {
    std::string s;
    BNBCommandParser::toString(*i, s);
    fprintf(f, "%s", s.c_str());
  }
  fclose(f);
  printf("Executing %s\n", BNB_SUBMIT_CMD);
  int stat = system(BNB_SUBMIT_CMD);
  if(WIFEXITED(stat)) {
    int rc = WEXITSTATUS(stat);
    printf("Exit code: %d\n", rc);
    exit(rc);
  } else {
    exit(-1);
  }
}

bool processResult(BNBGate* g)
{
  bool rv = false;
  printf("Processing results\n");
  //system("cat bnbout.scr");
  BNBGate* ng = new BNBFileGate(BNB_OUT_FILE, "null");
  ng->open();
  while(1) {
    BNBCommand cmd;
    if(ng->probe()) {
      if(!ng->get(cmd)) {
	continue;
      }
      else {
	if(cmd.mName != BNB_HELLO) {
	  printf("putting %s\n", cmd.mName.c_str());
	  g->put(cmd);
	}
	if(cmd.mName == BNB_EXIT_STATUS) {
	  if(cmd.mDsc == BNB_STATUS_SUCCESS) {
	    rv = true;
	  }
	}
      }
    } else 
      break;
  }
  if(rv) {
    BNBCommand cmd;
    cmd.mName = BNB_FINISH;
    g->put(cmd);
  }
  ng->close();
  return rv;
}

void process(BNBGate* g)
{
  std::vector<BNBCommand> cmdv;
  std::vector<BNBCommand> pcmdv;
  int pid = -1;
  g->open();
  printf("Gate opened!\n");
  fflush(stdout);
  BNBCommand cmd;
  g->put(cmd);
  printf("Hello command sent to gate!\n");
  fflush(stdout);
  while(1) {
    if(g->probe()) {
      if(!g->get(cmd))
	continue;
      printf("bnbgrid got command: %s\n", cmd.mName.c_str());
      fflush(stdout);
      if(cmd.mName == BNB_PUT_PROBLEM_DSC) {
	pcmdv.push_back(cmd);
      } else if(cmd.mName == BNB_CREATE_SOLVER) {
	pcmdv.push_back(cmd);
      } else if(cmd.mName == BNB_START_SOLVE) {
	cmdv.push_back(cmd);
	pid = fork();
	if(pid == 0) {
	  cmdv.insert(cmdv.begin(), pcmdv.begin(), pcmdv.end());
	  submitWork(cmdv);
	} else {
	  printf("Child process %d forked\n", pid);
	  cmdv.clear();
	}
      } else {
	cmdv.push_back(cmd);
      }
    } else {
      if(pid != -1) {
	int status;
	int id = waitpid(pid, &status, WNOHANG);
	//printf("status = %d of %d (pid = %d)\n", status, id, pid);
	if(id != 0) {
	  printf("Process %d Exited\n", id);
	  pid = -1;
	  if(WIFEXITED(status)) {
	    int rc = WEXITSTATUS(status);
	    printf("Process exit code: %d (status = %d)\n", rc, status);
	    if(rc){
	      printf("Process Failed\n");
	      break;
	    } else {
	      printf("Process Succeded\n", id);
	      if(processResult(g)) {
		printf("Results processed successfully\n");
	      } else {
		printf("Error results processing\n");
		break;
	      }
	    }
	  } else {
	    printf("Process %d Falied\n", id);
	    break;
	  }
	}
      }
      usleep(10);
    }
  }
  g->close();
}

main(int argc, char* argv[])
{
  BNBGate *g;
  if(std::string(argv[1]) == "-f")
    g = new BNBFileGate(argv[2], argv[3]);
  else
    g = new BNBSocketGate(argv[2], argv[3]);

  process(g);
}



