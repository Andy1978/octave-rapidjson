// Copyright (C) 2017 Andreas Weber <andy@josoansi.de>
//
// GNU Octave wrapper around RapidJSON (http://rapidjson.org/)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see
// <http://www.gnu.org/licenses/>.

#include <octave/oct.h>
#include <octave/oct-map.h>
#include <octave/Cell.h>

#include <iostream>

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

//#define DEBUG
#include "debug.h"

using namespace rapidjson;
using namespace std;

bool save_element (Writer<StringBuffer> &writer, const octave_value& tc);

DEFUN_DLD (save_json, args,, "save_json (obj)")
{
  if (args.length () != 1)
    print_usage ();

// I think ls-mat5.cc:save_mat5_binary_element
// and the other save functions are a good source to see
// how to traverse an octave-value

#if 0
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  writer.StartObject();
  writer.Key("hello");
  writer.String("world");
  writer.Key("t");
  writer.Bool(true);
  writer.Key("f");
  writer.Bool(false);
  writer.Key("n");
  writer.Null();
  writer.Key("i");
  writer.Uint(123);
  writer.Key("pi");
  writer.Double(3.1416);
  writer.Key("a");
  writer.StartArray();
  for (unsigned i = 0; i < 4; i++)
      writer.Uint(i);
  writer.EndArray();
  writer.EndObject();
  return ovl (s.GetString());
#endif

  StringBuffer s;
  Writer<StringBuffer> writer(s);

  save_element (writer, args(0));

  return ovl (s.GetString());
}

bool save_element (Writer<StringBuffer> &writer, const octave_value& tc)
{
 std::string cname = tc.class_name ();
  octave_stdout << "cname = " << cname << std::endl;
  
  //int32_t nr = tc.rows ();
  //int32_t nc = tc.columns ();

  if (tc.issparse ())
    {
      octave_stdout << "issparse, nnz = " <<  tc.nnz () << ", iscomplex = " << tc.iscomplex () << std::endl;
      error ("JSON can't handle sparse matrix, convert to full matrix first (using 'full')");
    }
  else if (tc.is_inline_function ())
    {
      error ("Can't store inline function into JSON");
    }
  else if (tc.isobject ())
    {
      error ("Can't store object into JSON");
    }
  else if (tc.is_string ())
    {
      octave_stdout << "is_string()" << std::endl;

      octave::unwind_protect frame;

      charMatrix chm = tc.char_matrix_value ();

      octave_idx_type nrow = chm.rows ();
      octave_idx_type ncol = chm.cols ();

      //OCTAVE_LOCAL_BUFFER (double, buf, ncol*nrow);

      for (octave_idx_type i = 0; i < nrow; i++)
        {
          std::string tstr = chm.row_as_string (i);
          //const char *s = tstr.data ();

          octave_stdout << tstr << std::endl;
        }

    }
  else if (tc.is_range ())
    {
      octave_stdout << "is_range()" << std::endl;
      Range r = tc.range_value ();
      //~ double base = r.base ();
      //~ double inc = r.inc ();
      //~ octave_idx_type nel = r.numel ();
      //~ for (octave_idx_type i = 0; i < nel; i++)
        //~ {
          //~ double x = base + i * inc;
          //~ os.write (reinterpret_cast<char *> (&x), 8);
        //~ }
      
      // convert to matrix
      save_element (writer, r.matrix_value());
    }
  else if (tc.is_real_scalar ())
    {
      octave_stdout << "is_real_scalar()" << std::endl;
      octave_stdout << "is_integer_type() = " << tc.is_integer_type() << std::endl;
      
      if (tc.is_integer_type ())
        writer.Int(tc.int_value ());
      else if (tc.is_real_type ())
        writer.Double (tc.double_value ());
      else if (tc.is_complex_type ())
        error ("complex not yet supported");

    }
  else if (tc.is_real_matrix ())
    {
      octave_stdout << "is_real_matrix()" << std::endl;
      NDArray m = tc.array_value ();

      octave_stdout << "numel=" << m.numel() << std::endl;
      octave_stdout << "ndims=" << m.ndims() << std::endl;
      
      // Die ersten 2 Dimensionen drehen
      Array<int> p(dim_vector(m.ndims(), 1));
      for (int k = 2; k < m.ndims(); ++k)
        p(k) = k;
       p(0) = 1;
       p(1) = 0;
        
      m.permute (p);
      
      dim_vector d = m.dims ();

      // Wieviel öffnende Klammern?
      for (int k = 0; k < d.length(); ++k)
        writer.StartArray();
      
      const double *pd = m.fortran_vec ();
      
      for (int k=0; k<m.numel(); ++k)
        {
          octave_stdout << pd[k] << endl;
           writer.Double (pd[k]);
          
          // hier müsste man den index ausrechnen und dann EndArray/StartArray einfügen
          
        }
      
      for (int k = 0; k < d.length(); ++k)
        writer.EndArray();

    }
  else if (tc.is_complex_scalar ())
    {
      octave_stdout << "is_complex_scalar()" << std::endl;
      //Complex tmp = tc.complex_value ();
      //~ os.write (reinterpret_cast<char *> (&tmp), 16);
    }
  else if (tc.is_complex_matrix ())
    {
      octave_stdout << "is_complex_matrix()" << std::endl;
      ComplexMatrix m_cmplx = tc.complex_matrix_value ();
      //~ Matrix m = ::real (m_cmplx);
      //~ std::streamsize n_bytes = 8 * static_cast<std::streamsize> (len);
      //~ os.write (reinterpret_cast<const char *> (m.data ()), n_bytes);
      //~ m = ::imag (m_cmplx);
      //~ os.write (reinterpret_cast<const char *> (m.data ()), n_bytes);
    }
  else if (tc.iscell ())
    {
      Cell cell = tc.cell_value ();
    }
 else if (tc.isstruct ())
    {
      octave_map m = tc.map_value ();

      //char buf[64];
      //int32_t maxfieldnamelength = max_namelen + 1;

      octave_idx_type nf = m.nfields ();
      octave_stdout << "isstruct(), nf = " << nf << std::endl;

      string_vector keys = m.keys ();

      // loop over keys
      for (octave_idx_type i = 0; i < nf; i++)
        {
          std::string key = keys(i);
           octave_stdout << "keys(" << i << ") = " << key << std::endl;

        }

      octave_idx_type len = m.numel ();
      octave_stdout << "len = " << len << std::endl;

      std::vector<const octave_value *> elts (nf);
      for (octave_idx_type i = 0; i < nf; i++)
        elts[i] = m.contents (keys(i)).data ();

      // loop over the elements
     for (octave_idx_type j = 0; j < len; j++)
      {
        // Iterating over the list of keys will preserve the order
        // of the fields.
        for (octave_idx_type i = 0; i < nf; i++)
          {
            //octave_stdout << elts[i][j] << std::endl;
            //octave_stdout << elts[i][j].double_value() << std::endl;
            octave_stdout << elts[i][j].class_name() << std::endl;
            octave_stdout << elts[i][j].matrix_value() << std::endl;
          }
      }
      
     // FIXME: soll man einen struct als
     // { "a": [1,2,3], "b" : [10,20,30]}
     // oder
     // [{ "a": 1, "b": 10},{ "a": 2, "b": 20},{ "a": 3, "b": 30}]
     //
     // ich glaube letzteres ist wohl bei JSONiander üblicher...
    
    }
  else
    error ("not yet implemented");
  
  octave_stdout << "-----------------------------------------" << std::endl;  
  
  return true;
}

/*
%!test
%! save_json (pi)

%!test
%! save_json ([2,3,4])

%!test
%! save_json (rand(1,2,3))

%!test
%! b(1).a = 4;
%! b(2).c = pi;
%! b(3).c = 2.718;
%! save_json (b)
* 
*/
