#include <octave/oct.h>
#include <octave/oct-map.h>

#define INITIAL_ARRAY_SIZE 2

using namespace std;

class dynNDArray
{
public:

  dynNDArray ()
//    : array (dim_vector (1, INITIAL_ARRAY_SIZE), -1)
    : real_size (0, 0),
      array (dim_vector (INITIAL_ARRAY_SIZE, INITIAL_ARRAY_SIZE), 0),
      write_pos (dim_vector (2, 1), 0),
      depth (0),
      last_was_cb (false)
  {
    //cout << "*******************************************" << endl;
  }

  //dim_vector capacity;
  dim_vector real_size;

  NDArray array;

  // index where the next value is written
  Array<int> write_pos;

  // "depth" of the current position
  // inc with [, dec with ]
  int depth;


  bool last_was_cb;


// folgt [ einer vorherigen ], wird der write_pos die Zeile inkrementiert
// folgt [ einer vorherigen [, wird eine neue Dimension angelegt und write_pos wird in der Dimension erhöht


  // opening bracket [
  void ob ()
  {
    depth++;

    if (last_was_cb) // '[' after ']'
      {
        // write_pos wächst ja mit depth, daher von hinten zählen
        int depth_index = write_pos.numel() - (depth - 1);
        write_pos (depth_index)++;

        // alle niedrigeren Dimensionen auf 0 setzen
        for (int k = 0; k < depth_index; ++k)
          write_pos (k) = 0;
      }
    else
      {
        if (depth > write_pos.numel())
          {
            int new_dims = write_pos.numel() + 1;
            //cout << "new_dims = " << new_dims << endl;
            write_pos.resize (dim_vector (new_dims, 1), 0);
            real_size.resize (new_dims);

            // add new dimension
            dim_vector s = array.dims ();
            //print_dim (s);
            //cout << " before" << endl;
            s.resize (new_dims);
            s(new_dims - 1) = INITIAL_ARRAY_SIZE;
            array.resize (s);

            //print_dim (array.dims ());
            //cout << " after" << endl;

          }
        //
        //print_write_pos ();
        //capacity.resize (dim_vector (new_dims, 0), 0);
        //array.resize (


        write_pos (0) = 0;
      }


    last_was_cb = false;

    //print_state ("ob() END");
  }

  // closing bracket
  void cb ()
  {
    depth--;
    last_was_cb = true;

    //print_state ("cb () ");

    // write_pos(0) was already incremented
    write_pos(0)--;

    // store actually needed size
    for (int k = 0; k < write_pos.numel(); ++k)
      if ((write_pos(k) + 1) > real_size(k))
        real_size(k) = write_pos(k) + 1;


    if (depth == 0)
      {
        array.resize (real_size);
        Array<int> p(dim_vector(real_size.ndims(), 1));
        for (int k = 2; k < real_size.ndims(); ++k)
          p(k) = k;
        p(0) = 1;
        p(1) = 0;

        array = array.permute (p);
      }
    //print_state ("cb()");
  }

  void value (double v)
  {
    // check if the array needs to be resized
    dim_vector s = array.dims ();
    //cout << "s(0)=" << s(0)  << " s(1)=" << s(1) << endl;
    for (int k = 0; k < s.ndims(); ++k)
      if (write_pos(k) > s(k) - 1)
        {
          //cout << "need resize in dim " << k << endl;
          // double it in this dimension
          //print_dim (s);
          //cout << " before" << endl;
          s(k) *= 2;
          array.resize (s);
          //print_dim (array.dims ());
          //cout << " after" << endl;

        }

    //cout << v;
    //print_state (" value");
    array (write_pos) = v;

    write_pos(0)++;
  }

  void print_state (const char *pre)
  {
    cout << pre << ", write_pos = [";
    for (int k=0; k<write_pos.numel(); ++k)
      {
        if (k > 0)
          cout << " ";
        cout << write_pos(k);
      }
    cout << "], depth =  " << depth << ", last_was_cb = " << last_was_cb << endl;
  }

  void print_dim (dim_vector s)
  {
    for (int k=0; k<s.ndims(); ++k)
      {
        if (k > 0)
          cout << " ";
        cout << s(k);
      }
  }

};

// {"a":[1,2,3]}
// {"b":[[1,2],[3,4],[5,6]]}
// {"c":[[[1,2],[3,4]],[[10,20],[30,40]]]}

DEFUN_DLD (doit, args,, "doit")
{
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


  // {"a":[1,2,3]}
  dynNDArray a;
  a.ob ();
  a.value (1);  // (0,0)
  a.value (2);  // (1,0)
  a.value (3);  // (2,0)
  a.cb ();

  // {"b":[[1,2],[3,4],[5,6]]}
  dynNDArray b;
  b.ob ();
  b.ob ();
  b.value (1);  // (0,0)
  b.value (2);  // (1,0)
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

  // {"c":[[[1,2],[3,4]],[[10,20],[30,40]]]}
  dynNDArray c;
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

  // {"d":[[[[1,2],[3,4],[5,6]],[[10,20],[30,40],[50,60]]],
  dynNDArray d;
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

  cout << "real_size = " << endl;
  for (int k = 0; k < d.real_size.ndims(); ++k)
    cout << d.real_size(k) << endl;
  //return ovl (a.array, b.array, c.array);
  //return ovl (a.array);
  return ovl(a.array, b.array, c.array, d.array);
}
