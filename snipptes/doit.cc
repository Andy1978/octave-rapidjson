#include <iostream>
#include <octave/oct.h>

#include <iostream>

using namespace std;

ostream& operator << (ostream& os, const dim_vector &rhs)
{
  os << "dim_vector";
  os << ": ndims = " << rhs.ndims();
  os << ", numel = " << rhs.numel();
  os << ", dims = [" << rhs.str(',');
  os << "]" << endl;;
  return os;
}


int
main (void)
{
  std::cout << "Hello Octave world!\n";

  //~ int n = 2;
  //~ Matrix a_matrix = Matrix (n, n);

  //~ for (octave_idx_type i = 0; i < n; i++)
  //~ for (octave_idx_type j = 0; j < n; j++)
  //~ a_matrix(i,j) = (i + 1) * 10 + (j + 1);

  //~ std::cout << a_matrix;

  //~ dim_vector a (2,3,4,1);
  //~ cout << a.redim (3) << endl;


  dim_vector b (4,2,3);
  octave_idx_type idx[] = {0, 0, 0};

  int r = b.ndims ();
  for (int i = 0; i < r; ++i)
    cout << "[";
  do
    {
      r = b.increment_index (idx, 0);
      cout << b.compute_index(idx) << ",";

      for (int i = 0; i < r; ++i)
        cout << "]";
      if (r != b.ndims () && r > 0)
        {
          cout << ",";
          for (int i = 0; i < r; ++i)
            cout << "[";
        }


      //cout << "r = " << r << ": " << idx[0] << " " << idx[1] << " " << idx[2] << endl;

    }
  while (r != b.ndims ());

  return 0;
}
