#include <sstream>
#include <util/common/util.hpp>
#include <ei/external/bnbfilegate.hpp>
#include <ei/external/bnbcmdparser.hpp>
#include <util/xml/xmlpars.hpp>
#include <ei/external/bnbdirgate.hpp>
#include <util/common/fileutils.hpp>

struct Folders {
  void setup() {
    char* dgdir = getenv("DGDIR");
    if(dgdir == NULL)
      BNB_ERROR_REPORT("DGDIR not set\n");
    mPrj = dgdir;
    mWu = dgdir;
    mWu += "/wu";
    mRes = dgdir;
    mRes += "/res";
  }
  std::string mWu;
  std::string mRes;
  std::string mPrj;
};

Folders folders;


void submitWork()
{
  std::string cmd = "cd ";
  cmd += folders.mPrj;
  cmd += "; ";
  cmd += "./submit.sh";
  system(cmd.c_str());
}

void clearWuFolder()
{
  FileUtils::removeFiles(folders.mWu, "unit*");
}

void prepareFiles(std::vector<BNBCommand>& cmdv, std::vector<std::string>& solv)
{
  std::ostringstream cos;
  while(cmdv.size() != 1) {
    BNBCommand cmd = cmdv.front();
    cmdv.erase(cmdv.begin());
    std::string s;
    BNBCommandParser::toString(cmd, s);
    cos << s;
  }
  clearWuFolder();
  while(!solv.empty()) {
    std::ostringstream os;
    os << folders.mWu << "/";
    os << "unit" << solv.size();
    FILE* f = fopen(os.str().c_str(), "w");
    os.clear();
    std::ostringstream oss;
    oss << cos.str();
    BNBCommand cmd;
    cmd.mName = BNB_PUT_SOLUTIONS;
    cmd.mDsc = "\n";
    cmd.mDsc += "<solu>";
    cmd.mDsc += solv.back();
    cmd.mDsc += "</solu>\n";
    solv.pop_back();
    std::string s;
    BNBCommandParser::toString(cmd, s);
    oss << s;
    BNBCommand ccmd = cmdv.front();
    std::string ss;
    BNBCommandParser::toString(ccmd, ss);
    oss << ss;
    fprintf(f, "%s", oss.str().c_str());
    fclose(f);
  }
}

void process(BNBGate* g, BNBGate* fg)
{
  std::string pt = "";
  std::vector<BNBCommand> cmdv;
  std::vector<std::string> solv;
  int nu = 0;
  g->open();
  while(1) {
    if(g->probe()) {
      BNBCommand cmd;
      if(!g->get(cmd))
	continue;
      std::string s;
      BNBCommandParser::toString(cmd, s);

      printf("got command: %s\n", s.c_str());
      if(cmd.mName == BNB_PUT_PROBLEM_DSC) {
	cmdv.push_back(cmd);
      } else if(cmd.mName == BNB_CREATE_SOLVER) {
	cmdv.push_back(cmd);
      } else if(cmd.mName == BNB_PUT_SOLVER_CFG) {
	cmdv.push_back(cmd);
      } else if(cmd.mName == BNB_SOLVE) {
	cmdv.push_back(cmd);
      } else if(cmd.mName == BNB_PUT_SOLUTIONS) {
	if(cmd.mDsc != BNB_NIL) {
	  class SP : public XMLListener {
	    public:
	      SP(std::vector<std::string>* sv)
	      {
		mSolv = sv;
	      }

	      void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
	      {
		if(tag == BNB_SOLUTION){
		  std::string str(s, begin, end - begin + 1);
		  printf("Pushing [%s]\n", str.c_str());
		  mSolv->push_back(str);
		}
	      }
	    private:
	      std::vector<std::string>* mSolv;
	  };

	  SP spl(&solv);
	  XMLPars pars;
	  pars.addListener(&spl);
	  pars.parse(cmd.mDsc);
	}
      } else if(cmd.mName == BNB_START_SOLVE) {
	cmdv.push_back(cmd);
	nu = solv.size();
	prepareFiles(cmdv, solv);
	submitWork();
      }
    } else {

      if(fg->probe()) {
	printf("getting command\n");
	BNBCommand cmd;
	if(fg->get(cmd)) {
	printf("putting command %s\n", cmd.mName.c_str());
	  g->put(cmd);
	  nu --;
	  if(nu <= 0)
	    break;
	}
      }
      usleep(10);
    }
  }
  g->close();
}

main(int argc, char* argv[])
{
  if(argc != 3)
    BNB_ERROR_REPORT("Usage: bnbss inputfile outputfile");
  folders.setup();
  BNBGate* g = new BNBFileGate(argv[1], argv[2]);
  BNBGate* fg = new BNBDirGate(folders.mRes, folders.mRes);
  process(g, fg);
}



