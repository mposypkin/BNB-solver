#ifndef __BNBCMDPARSER__HPP__
#define __BNBCMDPARSER__HPP__

#include <string>
#include "bnbcommand.hpp"
#include "bnbcmdtags.hpp"
#include <util/xml/xmlpars.hpp>

/**
 * Collcection of methods for parsing of bnb commands
 */
class BNBCommandParser {
  public:
    class PListener : public XMLListener {
      public:
        PListener(BNBCommand* cmd)
        {
          mCmd = cmd;
        }
        
      /**
       * Callback. Called when the closing tag is encountered.
       * @param level nesting level
       * @param tag tag
       * @param s the parsed string
       * @param begin begin contents position
       * @param end end contenets position
       */
      void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
      {
        std::map<std::string, std::string*> pm;
        std::map<std::string, std::string*>::iterator ipm;
        pm[BNB_COMMAND_NAME] = &(mCmd->mName);
        pm[BNB_COMMAND_TYPE] = &(mCmd->mType);
        pm[BNB_COMMAND_ID] = &(mCmd->mId);
        pm[BNB_COMMAND_STATUS] = &(mCmd->mStatus);
        pm[BNB_COMMAND_DESCRIPTION] = &(mCmd->mDsc);
        pm[BNB_COMMAND_CODE] = &(mCmd->mCode);
        
        ipm = pm.find(tag);
        if(ipm != pm.end()) {
            (*(ipm->second)).assign(s, begin, end  - begin + 1);
        }
      }
      
      private:
        
        BNBCommand* mCmd;
    };

    /**
     * Tranasformrms command to a string representation
     * @param cmd command
     * @param str string
     */
    static void toString(const BNBCommand& cmd, std::string& str)
    {
      std::ostringstream os;
      os << "\n" << BNB_COMMAND_START << "<" << BNB_COMMAND_COMMAND << ">\n";
      os << "  <" << BNB_COMMAND_NAME << ">";
      os << cmd.mName;
      os << "</" << BNB_COMMAND_NAME << ">\n";
      os << "  <" << BNB_COMMAND_ID << ">";
      os << cmd.mId;
      os << "</" << BNB_COMMAND_ID << ">\n";
      os << "  <" << BNB_COMMAND_TYPE << ">";
      os << cmd.mType;
      os << "</" << BNB_COMMAND_TYPE << ">\n";
      os << "  <" << BNB_COMMAND_STATUS << ">";
      os << cmd.mStatus;
      os << "</" << BNB_COMMAND_STATUS << ">\n";
      os << "  <" << BNB_COMMAND_DESCRIPTION << ">";
      os << cmd.mDsc;
      os << "</" << BNB_COMMAND_DESCRIPTION << ">\n";
      os << "  <" << BNB_COMMAND_CODE << ">";
      os << cmd.mCode;
      os << "</" << BNB_COMMAND_CODE << ">\n";
      os << "</" << BNB_COMMAND_COMMAND << ">" << BNB_COMMAND_END << "\n";
      str.assign(os.str());
    }
    
    /**
     * Tranasformrms command to a string representation
     * @param cmd command
     * @param str string
     */
    static std::string toString(const BNBCommand& cmd)
    {
      std::string s;
      toString(cmd, s);
      return s;
    }
    
    /**
     * Constructs a command from its string representation
     * @param str string
     * @param cmd command
     */
    static void fromString(const std::string& str, BNBCommand& cmd)
    {
      XMLPars pars;
      PListener lst(&cmd);
      pars.addListener(&lst);
      pars.parse(str);      
    }
};

#endif

