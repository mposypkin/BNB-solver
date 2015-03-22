#ifndef __XMLLISTENER_HPP__
#define __XMLLISTENER_HPP__

class XMLListener {
  public:
    /**
     * Callback. Called when the closing tag is encountered.
     * @param level nesting level (from 1 upto ...)
     * @param tag tag
     * @param s the parsed string
     * @param begin begin contents position
     * @param end end contenets position
     */
    virtual void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end) = 0;
};

#endif
