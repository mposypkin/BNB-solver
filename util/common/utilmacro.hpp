#ifndef __UTILMACRO_HPP__
#define __UTILMACRO_HPP__ 

/**
  * Useful macros
  */

#define CP(rank, str, del) {char name[128]; FILE* f; sprintf(name, "bnb.%d.log", rank);f = fopen(name, "a"); fprintf(f, "%s\n", str); fflush(f); fclose(f); sleep(del);}

#define BNBMAX(a,b) (((a)>(b))?(a):(b))

#define BNBMIN(a,b) (((a)<(b))?(a):(b))

#define BNBABS(a) BNBMAX((a), (-(a)))

#define BNBSQR(a) ((a) * (a))

#define BNBSGN(a) (((a) >= 0) ? 1 : -1)


#endif
