#ifndef __BNB_CMD_PROCESSOR_HPP__
#define __BNB_CMD_PROCESSOR_HPP__

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string>
#include <vector>
#include <map>
#include <util/common/bnberrcheck.hpp>
#include "bnbcommand.hpp"
#include "bnbactions.hpp"

/**
 * Process BNB commands
 */
class BNBCommandProcessor {
  public:
    /**
     * Process command
     * @param command command to process 
     * @param action resulting action
     */
    void process(BNBCommand& command, int& action)
    {
      action = BNBActions::NOACTION;
      mCurCommand = &command;
      mAction = &action;
      do {
        std::vector < std::string > args;
        parseCommand(command.mCode, args);
        processCommand(args);
      } while(action == BNBActions::NOACTION);
    }
    
  private:
    
    void parseCommand(std::string& command, std::vector<std::string>& args )
    {
      int i;
      std::string s;
      bool tr = false;
      bool quotes = false;
      i = 0;
      
      printf("Processing: [%s]\n", command.c_str());
      
      while(true) {
        char c;
        if(i == command.size()){
          if(tr) {
            args.push_back(s);
            s.clear();
          }
          break;
        }
        c = command[i];
        i ++;
        if((!quotes) && ((c == '\n') || (c == ' ') || (c == '\r'))) {       
          if(tr) {
            args.push_back(s);
            s.clear();
            tr = false;
          }
        } else if((!quotes) && (c == ';')) {
          if(tr) {
            args.push_back(s);
            s.clear();
          }
          break;
        } else if(c == '\"'){
          if(quotes)
            quotes = false;
          else
            quotes = true;
        } else {
          tr = true;          
          s += c;
        }
      }
    }
        
    void processCommand(std::vector< std::string > &args)
    {
/*      printf("PROCESS: ");
      for(int i = 0; i < args.size(); i ++) {
        printf("[%s] ", args[i].c_str());
      }
      printf("\n");*/
      if(args.empty()) {
        *mAction = BNBActions::ERASE;
      } else if(args[0] == "forward")  {
        *mAction = BNBActions::FORWARD | BNBActions::ERASE;
        eraseCommand();
      } else if(args[0] == "drop")  {
        *mAction = BNBActions::DROP | BNBActions::ERASE;
        eraseCommand();
      } else if(args[0] == "var") {
        BNB_ASSERT(args.size() == 2);
        processVar(args[1]);
      } else if(args[0] == "set") {
        BNB_ASSERT(args.size() == 3);
        processAssign(args[1], args[2]);
      } else if(args[0] == "inc") {
        BNB_ASSERT(args.size() == 3);
        processInc(args[1], args[2]);
      } else if(args[0] == "cat") {
        BNB_ASSERT(args.size() == 3);
        processCat(args[1], args[2]);
      } else if(args[0] == "wait") {
        BNB_ASSERT(args.size() == 4);        
        processWait(args[1], args[2], args[3]);
      } else if(args[0] == "echo") {
        processEcho(args);
      } else if(args[0] == "erase") {
        *mAction = BNBActions::ERASE;
        eraseCommand();        
      } else if(args[0] == "save") {
        processSave(args[1], args[2]);
      } else if(args[0] == "append") {
        processAppend(args[1], args[2]);
      } else if(args[0] == "load") {
        processLoad(args[1], args[2]);
      } else if(args[0] == "mkdir") {
        processMkdir(args[1]);
      } else if(args[0] == "skip") {
        eraseCommand();
      } else if(args[0] == "exec") {
        processExec(args[1]);
      } else if(args[0] == "exit") {
        *mAction = BNBActions::EXIT;
        eraseCommand();
      } else {
        printf("%s\n", args[0].c_str());
        BNB_ERROR_REPORT("Unrecognized command");
      }
    }
    
    void processEcho(std::vector< std::string> & args)
    {
      for(int i = 1; i < args.size(); i ++)
        printf("%s ", getIdent(args[i])->c_str());
      printf("\n");
      eraseCommand();
    }
    
    void processVar(std::string& lop)
    {
      mMemory[lop] = "";
      eraseCommand();
    }
    
    void processAssign(std::string& lop, std::string& rop)
    {
      *getIdent(lop) = *getIdent(rop);
      eraseCommand();
    }
    
    void processExec(std::string& evar) 
    {
      std::string cmd;
      std::vector< std::string > args;
      cmd = *getIdent(evar);
      eraseCommand();
      cmd.append(";");
      mCurCommand->mCode.insert(0, cmd);
      
    }
    
    void processInc(std::string& lop, std::string& rop)
    {
      std::string *lopv, *ropv;
      int ilop, irop;
      lopv = getIdent(lop);
      ropv = getIdent(rop);
      ilop = atoi(lopv->c_str());
      irop = atoi(ropv->c_str());
      std::ostringstream os;
      os << ilop + irop;
      lopv->assign(os.str());
      eraseCommand();
    }
    
    void processCat(std::string& lop, std::string& rop)
    {
      std::string *lopv, *ropv;
      lopv = getIdent(lop);
      ropv = getIdent(rop);
      std::ostringstream os;
      os << *lopv << *ropv;
      lopv->assign(os.str());
      eraseCommand();
    }
   
    void processWait(std::string& lop, std::string& sign, std::string &rop)
    {
      std::string *lopv, *ropv;
      int ilop, irop;
      lopv = getIdent(lop);
      ropv = getIdent(rop);
      ilop = atoi(lopv->c_str());
      irop = atoi(ropv->c_str());
      if(sign == "<") {
        if(ilop < irop) {
          eraseCommand();
        } else {
          *mAction = BNBActions::SKIP;
        }
      } else {
        BNB_ERROR_REPORT("Unsupported comparison");
      }
    }
        
    void processSave(std::string& lop, std::string& rop)
    {
      std::string *plop, *prop;
      FILE* f;
      plop = getIdent(lop);
      prop = getIdent(rop);
      f = fopen(prop->c_str(), "w");
      BNB_ASSERT(f);
      fprintf(f, "%s", plop->c_str());
      fclose(f);
      eraseCommand();
    }
    
    void processAppend(std::string& lop, std::string& rop)
    {
      std::string *plop, *prop;
      FILE* f;
      plop = getIdent(lop);
      prop = getIdent(rop);
      f = fopen(prop->c_str(), "a");
      BNB_ASSERT(f);
      fprintf(f, "%s", plop->c_str());
      fclose(f);
      eraseCommand();
    }
    

    void processLoad(std::string& lop, std::string& rop)
    {
      std::string *plop, *prop;
      FILE* f;
      plop = getIdent(lop);
      prop = getIdent(rop);
      plop->clear();
      f = fopen(prop->c_str(), "r");
      BNB_ASSERT(f);
      while(true) {
        int rc;
        unsigned char c;
        rc = fgetc(f);
        if(rc == EOF)
          break;
        else {
          c = (unsigned char) rc;
          *plop += c;
        }
      }
      fclose(f);
      eraseCommand();
    }

    void processMkdir(std::string& dir)
    {
      int rc;
      std::string* dname;      
      mode_t mod;
      dname = getIdent(dir);
      mod = 0777;
      rc = mkdir(dname->c_str(), mod);
      BNB_ASSERT(rc == 0);
      eraseCommand();
    }
    
    std::string* getIdent(std::string& ident)
    {
      BNB_ASSERT(ident.size());
      if(ident[0] == '$') {
        std::string nident, *var;
        nident.assign(ident, 1, ident.size() - 1);
        var = getIdent(nident);
        return getMem(var);
      } else if(ident[0] == '@') {
        std::string nident;
        nident.assign(ident, 1, ident.size() - 1);
        return getSpecVar(&nident);
      } else 
        return &ident;
    }
    
    std::string* getSpecVar(std::string* var)
    {
      std::string *rv;
      if(*var == "time") {
        evalTime();
        return &mTime;        
      } else if(*var == BNB_COMMAND_DESCRIPTION){
        rv = &(mCurCommand->mDsc);
      } else
        BNB_ERROR_REPORT("Unrecognized special variable");
      return rv;
    }
    
    std::string* getMem(std::string* var)
    {
      if(mMemory.find(*var) == mMemory.end())
        BNB_ERROR_REPORT("Variable does not exist");
      return &(mMemory[*var]);
    }
    
    void eraseCommand()
    {
      int i = 0;
      std::string& word = mCurCommand->mCode;
      bool quotes = false;
      
      while(true){
        if(i == word.size())
          break;
        else if(word[i] == '\"') {
          if(quotes)
            quotes = false;
          else
            quotes = true;
        } else if((!quotes) && (word[i] == ';')) {
          break;
        } 
        i ++;
      }
      word.erase(0, i + 1);
    }
    
    void evalTime()
    {
      std::ostringstream os;
      os << time(NULL);
      mTime.assign(os.str());
    }
    
    std::map <std::string, std::string> mMemory;
    BNBCommand*  mCurCommand;
    std::string mTime;
    int* mAction;
};
#endif
