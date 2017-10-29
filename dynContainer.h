#include <octave/oct.h>
#include <octave/oct-map.h>
#include <ostream>
#include <iomanip>

#define INITIAL_ARRAY_SIZE 2
#define GROWTH_FACTOR 1.5

#define DEBUG

using namespace std;

template <typename base_container> 
class dynContainer
{
private:

  dim_vector real_size;   // The really used size in each dimension.
                          // It's always smaller than the reserved size in "array"

  base_container array;   // typically "NDArray" or "Cell"

  Array<int> write_pos;   // position where the next call to "value" is written

  int depth;              // "depth" of the current position
                          // increments with "ob", decrements with "cb"

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

  void print_state (const char *token)
  {
#define FW 14
    static int header = 1;
    if (header)
      {

        cout << setw (8) << "token";
        cout << setw (6) << "depth";
        cout << setw (12) << "last_was_cb";
        cout << setw (FW) << "write_pos";
        cout << setw (FW) << "real_size";
        cout << setw (FW) << "array.dims()";
        cout << endl;
      }

    cout << setw (8) << token;
    cout << setw (6) << depth;
    cout << setw (12) << last_was_cb;
    cout << setw (FW) << vector_to_string (write_pos);
    cout << setw (FW) << vector_to_string (real_size);
    cout << setw (FW) << vector_to_string (array.dims());
    cout << endl;

    header = 0;
  }
#endif

public:

  dynContainer ()
    : real_size (0, 0),
      array (dim_vector (INITIAL_ARRAY_SIZE, INITIAL_ARRAY_SIZE), lo_ieee_na_value ()),
      write_pos (dim_vector (2, 1), 0),
      depth (0),
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
            dim_vector s = array.dims ();
            s.resize (new_dims);
            s(new_dims - 1) = INITIAL_ARRAY_SIZE;
            array.resize (s);
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
        array.resize (real_size);

        // Die ersten 2 Dimensionen drehen
        Array<int> p(dim_vector(real_size.ndims(), 1));
        for (int k = 2; k < real_size.ndims(); ++k)
          p(k) = k;
        p(0) = 1;
        p(1) = 0;
        array = array.permute (p);
      }
#ifdef DEBUG
    print_state ("]");
#endif
  }

  // insert value at current position
  void value (double v)
  {
    // check if the array needs to be resized
    dim_vector s = array.dims ();
    //cout << "s(0)=" << s(0)  << " s(1)=" << s(1) << endl;
    for (int k = 0; k < s.ndims(); ++k)
      if (write_pos(k) > s(k) - 1)
        {
          s(k) = ceil (s(k) * GROWTH_FACTOR);
          array.resize (s);
        }
    array (write_pos) = v;

    write_pos(0)++;

#ifdef DEBUG
    char buf[10];
    snprintf (buf, 10, "%.1f", v);
    print_state (buf);
#endif
  }

  base_container const& get_array ()
  {
    return array;
  }
  
  int get_depth ()
  {
    return depth;
  }
};
