#include <octave/oct.h>
#include <octave/oct-map.h>
#include "dynContainer.h"

DEFUN_DLD (doit, args,, "doit")
{
  (void) args;
#if 0
  dim_vector dim = dim_vector(3,4,5,6);
  NDArray P = NDArray(dim, 0);

  Array<int> coord (dim_vector (4, 1), 0);
  coord (0, 0) = 1;
  coord (1, 0) = 2;
  coord (2, 0) = 3;
  coord (3, 0) = 4;

  P(coord) = 123;;

  RowVector x(4);
  x(0) = 2;
  x(1) = 2;
  x(2) = 2;
  x(3) = 2;
  P(x) = 444;
#endif
  /******************************************/

  //~ dim_vector dim;
  //~ NDArray tmp = NDArray(dim, 0);

  //~ // [
  //~ dim.resize (1);
  //~ dim(0) = 5;     // initial size

  //~ tmp.resize (dim, 0);

  //~ // 10
  //~ tmp (0, 1) = 10;
  //~ // 20
  //~ tmp (0, 2) = 20;

#if 1
  // {"a":[1,2,3]}
  dynContainer<NDArray> a;
  a.ob ();
  a.value (1);  // (0,0)
  a.value (2);  // (1,0)
  a.value (3);  // (2,0)
  a.cb ();

  // {"b":[[1,2],[3,4],[5,6]]}
  //dynContainer<Cell> b;
  dynContainer<NDArray> b;

  b.ob ();
  b.ob ();
  b.value (1);  // (0,0)
  //b.value (2);  // (1,0)
  b.cb ();
  b.ob ();
  b.value (3);  // (0,1)
  b.value (4);  // (1,1)
  b.cb ();
  b.ob ();
  b.value (5);  // (0,2)
  b.value (6);  // (1,2)
  b.cb ();
  b.cb ();
  
  dynContainer<Cell> c;
  c = b;

  //dynContainer<NDArray> d;
  //d = a;
#endif

#if 0
  // {"c":[[[1,2],[3,4]],[[10,20],[30,40]]]}
  dynContainer c;
  c.ob ();
  c.ob ();
  c.ob ();
  c.value (1);  // (0,0,0)
  c.value (2);  // (1,0,0)
  c.cb ();
  c.ob ();
  c.value (3);  // (0,1,0)
  c.value (4);  // (1,1,0)
  c.cb ();
  c.cb ();
  c.ob ();
  c.ob ();
  c.value (10);  // (0,0,1)
  c.value (20);  // (1,0,1)
  c.cb ();
  c.ob ();
  c.value (30);  // (0,1,1)
  c.value (40);  // (1,1,1)
  c.cb ();
  c.cb ();
  c.cb ();
#endif

#if 0
  // {"d":[[[[1,2],[3,4],[5,6]],[[10,20],[30,40],[50,60]]],
  dynContainer<Cell> d;
  //dynContainer<NDArray> d;
  d.ob ();
  d.ob ();
  d.ob ();
  d.ob ();
  d.value (1);
  d.value (2);
  d.cb ();
  d.ob ();
  d.value (3);
  d.value (4);
  d.cb ();
  d.ob ();
  d.value (5);
  d.value (6);
  d.cb ();
  d.cb ();
  d.ob ();
  d.ob ();
  d.value (10);
  d.value (20);
  d.cb ();
  d.ob ();
  d.value (30);
  d.value (40);
  d.cb ();
  d.ob ();
  d.value (50);
  d.value (60);
  d.cb ();
  d.cb ();
  d.cb ();

  // [[[100,200],[300,400],[500,600]],[[1000,2000],[3000,4000],[5000,6000]]]]}'

  d.ob ();
  d.ob ();
  d.ob ();
  d.value (100);
  d.value (200);
  d.cb ();
  d.ob ();
  d.value (300);
  d.value (400);
  d.cb ();
  d.ob ();
  d.value (500);
  d.value (600);
  d.cb ();
  d.cb ();
  d.ob ();
  d.ob ();
  d.value (1000);
  d.value (2000);
  d.cb ();
  d.ob ();
  d.value (3000);
  d.value (4000);
  d.cb ();
  d.ob ();
  d.value (5000);
  d.value (6000);
  d.cb ();
  d.cb ();
  d.cb ();
  d.cb ();

#endif

  return ovl (c.get_array ());
  //return ovl (a.get_array (), b.get_array ());
  //return ovl (a.array, b.array, c.array);
  //return ovl (a.array);
  //return ovl(a.array, b.array, c.array, d.array);
}
