#include <util/common/util.hpp>
#include <ei/external/bnbfilegate.hpp>
#include <ei/factory/prfact.hpp>
#include <ei/factory/sersolvfact.hpp>
#include <ei/external/bnbcmdprocessor.hpp>

BNBLog<BNB_DO_LOGGING> logger;

void process(BNBGate* g)
{
  BNBProblemData* pd = NULL;
  SerialSolver* solver = NULL;
  logger << "Processing started\n";
  logger.write();
  g->open();
  BNBCommand ocmd;
  g->put(ocmd);
  while(1) {
    if(g->probe()) {
      BNBCommand cmd;
      if(!g->get(cmd))
	continue;
      logger << "command from file gate: " << cmd.mName << "\n";
      logger.write();
      int action;
      BNBCommandProcessor cmdp;
      cmdp.process(cmd, action);
      if(cmd.mName == BNB_PUT_PROBLEM_DSC) {
	pd = BNBProblemFactory::createProblemData(cmd.mDsc);
      } else if(cmd.mName == BNB_CREATE_SOLVER) {
	if(pd) {
	  solver = BNBSerialSolverFactory::createSolver(pd, cmd.mDsc);
	  solver->setGate(g);
	}
	else
	  BNB_ERROR_REPORT("trying to create solver before problem data is created");
      } else if(cmd.mName == BNB_PUT_SOLVER_CFG) {
	  BNB_ERROR_REPORT("configure not supported");
      } else if(cmd.mName == BNB_SOLVE) {
	if(solver)
	  solver->solve();
	else
	  BNB_ERROR_REPORT("trying to run non-existent solver");
      }
    } else 
      break;
  }
  g->close();
  logger << "Processing finished\n";
  logger.write();
}

main(int argc, char* argv[])
{
  if(argc != 3) 
    BNB_ERROR_REPORT("Usage: bnbss inputfile outputfile");
  logger.setName("bnbss");
//  logger.setFile("stderr");
  BNBGate* g = new BNBFileGate(argv[1], argv[2]);
  process(g);
}
