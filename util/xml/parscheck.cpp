#include "xmlpars.hpp"

class My : public XMLListener {
  public:
  void fireTag(int level, const std::string& tag, const std::string& s, int begin, int end)
  {
    printf("tag: %s at level %d\n", tag.c_str(), level);
    printf("contents:");
    for(int i = begin; i <= end; i ++)
      printf("%c", s[i]);
    printf("\n");         
  }
};


main()
{
  XMLPars pars;
  My A;
  pars.addListener(&A);
  pars.parse("<family>"
               "<mama>Mama</mama>"
               "<papa>Papa</papa>"
               "<son>John</son>"
              "</family>");
  
  pars.parse("<family>"
      "<mama>Mama</mama>"
      "<papa>Papa</papa>"
      "<son>John</son>"
      "</faily>");
}
