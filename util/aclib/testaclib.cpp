#include <mpi.h>
#include <stdio.h>
#include "aclib.hpp"
#include "mpiaclib.hpp"

main (int argc, char *argv[])
{


  AbstractCommunicator ac (&argc, &argv);
  if (ac.getMyId () == 1)
    {
      std::string s ("xxx");
      ac << 11 << 22 << 33 << s;
      int a[3];

      ac.recv (1);
      ac >> a[0];
      ac >> a[1];
      ac >> a[2];
      ac >> s;
      printf ("%d=11, %d=22, %d=33, \"%s\" = \"xxx\"\n", a[0], a[1], a[2],
	      s.c_str ());
      fflush (stdout);
      ac.send (0);

      std::vector < double >v;

      v.push_back (0.1);
      v.push_back (0.2);
      v.push_back (0.3);
      ac << v;

      std::map < std::string, int >m;
      m["1"] = 1;
      m["2"] = 2;
      m["3"] = 3;

      ac << m;
      ac.send (0);
    }
  else if (ac.getMyId () == 0)
    {
      int a[3];
      std::string s;
      ac.recv (1);
      ac >> a[0];
      ac >> a[1];
      ac >> a[2];
      ac >> s;

      printf ("%d=11, %d=22, %d=33, \"%s\" = \"xxx\"\n", a[0], a[1], a[2],
	      s.c_str ());
      fflush (stdout);

      ac.recv (1);
      std::vector < double >v;
      ac >> v;

      printf ("0.1=%f 0.2=%f 0.3=%f\n", v[0], v[1], v[2]);
      fflush (stdout);

      std::map < std::string, int >m;
      ac >> m;

      printf ("1=%d, 2=%d, 3=%d\n", m["1"], m["2"], m["3"]);
      fflush (stdout);
    }


  if (ac.getMyId () == 0)
    {
      std::string s ("111");
      ac << s;
    }
  ac.bcast (0);

  std::string s;
  ac >> s;
  printf ("\"%s\" = \"111\"\n", s.c_str ());
  fflush (stdout);
  ac.barrier ();

  if (ac.getMyId () == 1)
    {
      try
      {
	ac.recv (0, -1);
      }
      catch (AbstractCommunicatorException e)
      {
	printf ("Exception \"%s\" caught\n", e.getInfo ());
	try
	{
	  ac.recv (0, (int) (1.5 * 1024 * 1024 * 1024));
	}
	catch (AbstractCommunicatorException e)
	{
	  printf ("Exception \"%s\" caught\n", e.getInfo ());
	  MPI_Abort (MPI_COMM_WORLD, -1);
	}
      }
    }
}
