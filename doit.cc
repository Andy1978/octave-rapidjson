#include <octave/oct.h>
#include <octave/oct-map.h>

#define INITIAL_ARRAY_SIZE 4

using namespace std;

class dynNDArray
{
public:
  
  dynNDArray ()
//    : array (dim_vector (1, INITIAL_ARRAY_SIZE), -1)
    : capacity (0, 0),
      array (capacity, -1),
      write_pos (dim_vector (2, 1), 0),
      depth (0),
      last_was_cb (false)
    {
      cout << "*******************************************" << endl;
    }

  dim_vector capacity;
  //dim_vector size;

  NDArray array;

  // index where the next value is written
  Array<int> write_pos;   

  // "depth" of the current position
  // inc with [, dec with ]
  int depth;


  bool last_was_cb;


// folgt [ einer vorherigen ], wird der write_pos die Zeile inkrementiert
// folgt [ einer vorherigen [, wird eine neue Dimension angelegt und write_pos wird in der Dimension erhöht  


  // opening bracket
  void ob ()
  {
    depth++;
    cout << "ob (), depth = " << depth << ", last_was_cb = " << last_was_cb 
         << " capacity.ndims () = " << capacity.ndims () << endl;
    
    // check if we need to add a dimension
    
    if (last_was_cb)
      {
        write_pos (depth - 1)++;
      }
    else
      {
        if (depth > 2)
          {
            int new_dims = write_pos.numel() + 1;
            cout << "new_dims = " << new_dims << endl;
            write_pos.resize (dim_vector (new_dims, 1), 0);
          }
        //
        //print_write_pos ();
        //capacity.resize (dim_vector (new_dims, 0), 0);
        //array.resize (
         //write_pos (depth - 1) = 0;
         //write_pos (depth)++;
        
      }
    write_pos (0) = 0;
    
    cout << "ob ";
    print_write_pos ();
    last_was_cb = false;
  }
  
  // closing bracket
  void cb ()
  {
    depth--;
    cout << "cb (), depth = " << depth << endl;
    
    last_was_cb = true;
    
  }
  
  void value (double v)
  {
    print_write_pos ();
    //array (write_pos) = v;
    write_pos(0)++;
  }

  void print_write_pos ()
  {
    cout << "write_pos = [";
    for (int k=0; k<write_pos.numel();++k)
      {
        if (k > 0)
          cout << " ";
        cout << write_pos(k);
      }
    cout << "]" << endl;
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

  //return ovl (a.array, b.array, c.array);
  return ovl (a.array);
}
