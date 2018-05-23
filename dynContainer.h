#include <octave/oct.h>
#include <octave/oct-map.h>
#include <ostream>
#include <iomanip>

#define INITIAL_ARRAY_SIZE 2
#define GROWTH_FACTOR 1.5

//#define DEBUG

//is_numeric_type was deprecated in 4.3.0, see libinterp/octave-value/ov.h
#if OCTAVE_MAJOR_VERSION == 4 && OCTAVE_MINOR_VERSION > 2 || OCTAVE_MAJOR_VERSION > 4
  #define ISNUMERIC(v) v.isnumeric ()
#else
  #define ISNUMERIC(v) v.is_numeric_type ()
#endif


using namespace std;

class dynContainer
{
private:

  NDArray DynArray;
  Cell DynCell;

  dim_vector real_size;   // The really used size in each dimension.
                          // It's always smaller than the reserved size in "array"

  Array<int> write_pos;   // position where the next call to "value" is written

  int depth;              // "depth" of the current position
                          // increments with "ob", decrements with "cb"

  bool array_is_numeric;  // the current processed array consists purly of
                          // numeric values and can thus be mapped to "NDArray".
                          // If false it's mapped to "Cell"

  bool last_was_cb;       // The last processed square bracked was "cb"

#ifdef DEBUG
  template <typename T>
  string vector_to_string (T d)
  {
    ostringstream tmp;
    tmp << "[";
    for (int k=0; k<d.ndims(); ++k)
      {
        if (k > 0)
          tmp << ",";
        tmp << d(k);
      }
    tmp << "]";
    return tmp.str ();
  }

  template <typename T>
  void print_state (T pre)
  {
    static int header = 1;
    if (header)
      {

        cout << setw (8) << "pre";
        cout << setw (6) << "depth";
        cout << setw (12) << "last_was_cb";
        cout << setw (14) << "write_pos";
        cout << setw (14) << "real_size";
        cout << setw (18) << "DynArray.dims()";
        cout << setw (18) << "DynCell.dims()";
        cout << endl;
      }

    cout << setw (8) << pre;
    cout << setw (6) << depth;
    cout << setw (12) << last_was_cb;
    cout << setw (14) << vector_to_string (write_pos);
    cout << setw (14) << vector_to_string (real_size);
    cout << setw (18) << vector_to_string (DynArray.dims());
    cout << setw (18) << vector_to_string (DynCell.dims());
    cout << endl;

    header = 0;
  }
#endif

  void resize (dim_vector d)
    {
      //cout << "resize called " << vector_to_string(d) << endl;

      // make empty matrix with size [0, 1] => [0, 0]
      if (d.numel () == 0 && d(1) == 1)
        d(1) = 0;

      if (array_is_numeric)
        DynArray.resize (d);
      else
        DynCell.resize (d);
    }

  dim_vector dims ()
    {
      return (array_is_numeric)? DynArray.dims ()
                               : DynCell.dims ();
    }

  // checks if the array needs to be resized
  // in the current write_pos dimension
  void resize_before_append ()
    {
      //cout << "resize_before_append called" << endl;
      dim_vector s = dims ();
      
      //cout << "s(0)=" << s(0)  << " s(1)=" << s(1) << endl;
      for (int k = 0; k < s.length(); ++k)
        if (write_pos(k) > s(k) - 1)
          {
            s(k) = ceil (s(k) * GROWTH_FACTOR);
            resize (s);
          }
    }
         
public:

  dynContainer ()
    : DynArray (dim_vector (INITIAL_ARRAY_SIZE, INITIAL_ARRAY_SIZE), lo_ieee_na_value ()),
      DynCell (dim_vector (INITIAL_ARRAY_SIZE, INITIAL_ARRAY_SIZE)),
      real_size (0, 0),
      write_pos (dim_vector (2, 1), 0),
      depth (0),
      array_is_numeric (true),
      last_was_cb (false)
  {
#ifdef DEBUG
  cout << "*******************************************" << endl;
#endif
  }

  // opening bracket
  void ob ()
  {
    depth++;

    // folgt '[' einer vorherigen ']', wird der write_pos die Zeile inkrementiert
    if (last_was_cb)
      {
        // write_pos wächst ja mit depth, daher von hinten zählen
        int depth_index = write_pos.numel() - (depth - 1);
        write_pos (depth_index)++;

        // alle niedrigeren Dimensionen auf 0 setzen
        for (int k = 0; k < depth_index; ++k)
          write_pos (k) = 0;
      }
    else
    // folgt '[' einer vorherigen '[', wird ggf. eine neue Dimension angelegt und write_pos wird in der Dimension erhöht
      {
        if (depth > write_pos.numel())
          {
            int new_dims = write_pos.numel() + 1;
            write_pos.resize (dim_vector (new_dims, 1), 0);
            real_size.resize (new_dims);

            // add new dimension to array
            dim_vector s = dims ();
            s.resize (new_dims);
            s(new_dims - 1) = INITIAL_ARRAY_SIZE;
            resize (s);
          }
        write_pos (0) = 0;
      }

#ifdef DEBUG
    print_state ("[");
#endif
    last_was_cb = false;
  }

  // closing bracket
  void cb ()
  {
    depth--;
    last_was_cb = true;

    // write_pos(0) was already incremented
    write_pos(0)--;

    // store actually needed size
    for (int k = 0; k < write_pos.numel(); ++k)
      if ((write_pos(k) + 1) > real_size(k))
        real_size(k) = write_pos(k) + 1;

    if (depth == 0)
      {
        resize (real_size);
      }
#ifdef DEBUG
    print_state ("]");
#endif
  }

  // insert value at current position
  void append_value (octave_value v)
    {
      //print_state ("append");
      if (! ISNUMERIC(v))
        {
          if (array_is_numeric)
            {
              // copy NDArray to Cell
              array_is_numeric = false;
              
              //cout << "Copy NDArray -> Cell" << endl;
              
              resize (DynArray.dims ());
              octave_idx_type nel = DynArray.numel ();
              //cout << "nel = " << nel << endl;
              for (octave_idx_type i = 0; i < nel; i++)
                DynCell(i) = DynArray(i);

              //cout << "copy done" << endl;
                    
              // FIXME: clear/resize DynArray
              // to free memory
            }
        }
#ifdef DEBUG
       if (v.is_string ())
         print_state (v.string_value ());
       else if (ISNUMERIC(v))
         print_state (v.double_value ());
#endif

      resize_before_append ();
      
      if (array_is_numeric)
        DynArray(write_pos) = v.double_value ();
      else
        DynCell(write_pos) = v;
      
      write_pos(0)++;
  }
  
  octave_value get_array ()
  {
    // Die ersten 2 Dimensionen drehen
    Array<int> p(dim_vector(real_size.length(), 1));
    for (int k = 2; k < real_size.length(); ++k)
      p(k) = k;
     p(0) = 1;
     p(1) = 0;
        
    if (array_is_numeric)
      return DynArray.permute (p);
    else
      return DynCell.permute (p);
  }
  
  int get_depth ()
  {
    return depth;
  }
};

