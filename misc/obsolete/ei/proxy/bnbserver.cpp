/**
 * BNB Server program
 * Allows to run and communicate BNB-Solver remotely.
 */

#include "bnbserver.hpp"

main(int argc, char* argv[])
{
  BNBServer server;

  server.parsArgs(argc, argv);
  server.mainLoop();
  
}
