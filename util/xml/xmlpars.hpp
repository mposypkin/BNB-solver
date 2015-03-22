#ifndef __XMLPARS_HPP__
#define __XMLPARS_HPP__

#include <stdio.h>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <algorithm>
#include <util/common/bnberrcheck.hpp>
#include "xmllistener.hpp"

class XMLPars {
  public:
    
    struct States {
      static const int READ = 1;
      static const int READ_TAG = 1 << 1;
      static const int END_TAG = 1 << 2;
    };
    
    struct TagRecord {
      std::string mTag;
      int mPosition;
    };
    
    /**
     * Parse a string represeantation of xml document
     * @param s string
     */
    void parse(const std::string& s)
    {
      int n, position, state;
      std::string curtag;
      n = s.size();
      state = States::READ;
      for(int i = 0; i < n; i ++) {
        char c;
        c = s[i];
        switch(c) {
          case '<':
            curtag.erase();
            state = States::READ_TAG;
            position = i - 1;
            break;
          case '/':            
            if(state == States::READ_TAG){
              if(s[i + 1] == '>') {
                curtag.erase();
                state = States::READ;
                i ++;
                break;
              } else {
                curtag.erase(); 
                state |= States::END_TAG;     
              }
            }     
            break;
          case '>':
            if(state & States::READ_TAG) {
              if(state & States::END_TAG){
                TagRecord& tr = mTags.top();
                if(tr.mTag != curtag)
                  BNB_ERROR_REPORT("Synatax error: tag unmached");
                fireTag(mTags.size(), tr.mTag, s, tr.mPosition, position);
                mTags.pop();
              } else {
                TagRecord tr;
                tr.mTag = curtag;
                tr.mPosition = i + 1;
                mTags.push(tr);
                state = States::READ;
              }
            }            
            break;
          default:
            if(state & States::READ_TAG) {
              curtag += c;
            }
        } 
      }
    }
    
    /**
     * Parse a string represeantation of xml document.
     * Generate the top-level structure.
     * @param s string
     * @param mp resulting map
     */
    void parse(const std::string& s, std::map<std::string, std::string>& mp)
    {
      class Lst : public XMLListener {
        public:
          Lst(std::map<std::string, std::string>* mp)
          {
            mMp = mp;
          }
          void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
          {
            if(level == 1) {
              std::string val;
              val.assign(s, begin, end - begin + 1);
              (*mMp)[tag] = val;
            }
          }
        private:
          std::map<std::string, std::string>* mMp;
      };
      Lst lst(&mp);
      addListener(&lst);
      parse(s);
      removeListener(&lst);
    }
    
    
    /**
     * Add a listener
     * @param listener listner to add
     */
    void addListener(XMLListener* listener)
    {
      std::vector <XMLListener*>::iterator i; 
      i = std::find(mListeners.begin(), mListeners.end(), listener);
      if(i != mListeners.end())
        BNB_ERROR_REPORT("Trying to add existing listener");
      mListeners.push_back(listener);
    }
    
    /**
     * Remove a listener
     * @param listener listner to remove
     */
    void removeListener(XMLListener* listener)
    {
      std::vector <XMLListener*>::iterator i; 
      i = std::find(mListeners.begin(), mListeners.end(), listener);
      if(i == mListeners.end())
        BNB_ERROR_REPORT("Trying to remove not-in-list listener");
      mListeners.erase(i);
    }
    
    
  private:
    
   void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
   {
     std::vector < XMLListener* >::iterator l;
     for(l = mListeners.begin(); l != mListeners.end(); l ++)
       (*l)->fireTag(level, tag, s, begin, end);
   }
   
   std::stack < TagRecord> mTags;
   std::vector < XMLListener* > mListeners;
};

#endif
