#include "dynContainer.h"

int main()
{
  dynContainer a;
  a.ob ();
  a.ob ();
  a.append_value (1.6);
  a.cb ();
  a.ob ();
  a.append_value (2.6);
  a.cb ();
  a.ob ();
  a.append_value (3.6);
  a.cb ();
  a.cb ();
  octave_value reta = a.get_array ();
  assert (reta.matrix_value()(0,0) == 1.6);
  assert (reta.matrix_value()(1,0) == 2.6);
  assert (reta.matrix_value()(2,0) == 3.6);

  dynContainer b;
  b.ob ();
  b.append_value (1.6);
  b.append_value (2.6);
  b.append_value (3.6);
  b.append_value ("huhu");
  b.append_value (5.6);
  b.cb ();
  octave_value retb = b.get_array ();

  return 0;
}
