/**
 * @author Mikhail Posypkin posypkin@ispras.ru
 *
 * @warning Using the code below requires an explicit permition from the author
 * @warning Any other use is prohibited
 *
 * @date Copyright by Mikhail Posypkin 2005-2015
 * @file bnbsolver.cpp
 *
 * Main BNB solver utility. Use for invocation solver classes for different problem 
 * types.
 */

#include <stdio.h>
#include <string>
#include <ei/external/bnbei.hpp>
#include <ei/external/bnbfilegate.hpp>
#include <ei/external/bnbsocketgate.hpp>
#include <util/aclib/mpiaclib.hpp>

main(int argc, char* argv[])
{
  BNBGate * g;
  AbstractCommunicator ac(&argc,&argv);
  
  if(argc != 4) {
    fprintf(stderr, "Usage: \n\t %s -f inputfile outputfile or \n\t %s -s agent_ip port\n", argv[0], argv[0]);
    exit(-1);
  }

  if(std::string(argv[1]) == "-f") 
    g = new BNBFileGate(argv[2], argv[3]);
  else
    g = new BNBSocketGate(argv[2], argv[3]);
  
  BNBEI ei(g, &ac);
  ei.solve();
  delete g;
}
