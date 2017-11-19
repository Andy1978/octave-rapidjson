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


dim_vector nsqueeze (dim_vector in)
{
  dim_vector new_dims = in;
  new_dims.chop_all_singletons();

  // preserve orientation if there is only one non-singleton dimension left
  if (new_dims.ndims () == 2 && in.elem(0) == 1 && new_dims.elem(1) == 1)
    return new_dims.as_row ();

  return new_dims;
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

  cout << nsqueeze (dim_vector (1,2,1,3)) << endl;
  //~ dim_vector a (1,2,1);
  //~ a.chop_all_singletons();
  //~ cout << a << endl;
  
  
  
  assert (dim_vector (0,0).squeeze () == dim_vector (0,0));
  assert (dim_vector (0,1).squeeze () == dim_vector (0,1));
  assert (dim_vector (1,1).squeeze () == dim_vector (1,1));
  assert (dim_vector (2,1).squeeze () == dim_vector (2,1));
  assert (dim_vector (1,2).squeeze () == dim_vector (1,2));
  assert (dim_vector (1,2,1).squeeze () == dim_vector (1,2));
  assert (dim_vector (1,2,1,3).squeeze () == dim_vector (2,3));
  assert (dim_vector (2,1,3).squeeze () == dim_vector (2,3));
  assert (dim_vector (1,1,3).squeeze () == dim_vector (1,3));
  

  assert (nsqueeze (dim_vector (0,0)) == dim_vector (0,0));
  assert (nsqueeze (dim_vector (0,1)) == dim_vector (0,1));
  assert (nsqueeze (dim_vector (1,1)) == dim_vector (1,1));
  assert (nsqueeze (dim_vector (2,1)) == dim_vector (2,1));
  assert (nsqueeze (dim_vector (1,2)) == dim_vector (1,2));
  assert (nsqueeze (dim_vector (1,2,1)) == dim_vector (1,2));
  assert (nsqueeze (dim_vector (1,2,1,3)) == dim_vector (2,3));
  assert (nsqueeze (dim_vector (2,1,3)) == dim_vector (2,3));
  assert (nsqueeze (dim_vector (1,1,3)) == dim_vector (1,3));


  //cout << d1 << endl;
  //cout << d1.squeeze() << endl;

  return 0;
}
