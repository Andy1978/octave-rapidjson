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

#if 0
  StringBuffer s;
  Writer<StringBuffer> writer(s);
//  writer.StartObject();
  writer.StartArray();
  writer.StartObject();
  writer.Key("hello");
  writer.String("world");
  writer.EndObject();
  writer.EndArray();
//  writer.EndObject();
  return ovl (s.GetString());
#endif

#if 0
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  writer.StartArray();
  writer.StartArray();
  for (unsigned i = 0; i < 4; i++)
    writer.Uint(i);
  writer.EndArray();
  writer.EndArray();
  return ovl (s.GetString());
#endif


#if 1
  StringBuffer s;
  Writer<StringBuffer> writer(s);
  save_element (writer, args(0));
  return ovl (s.GetString());
#endif

}

bool save_element (Writer<StringBuffer> &writer, const octave_value& tc)
{
  octave_stdout << "-------------- save_element --------------------" << std::endl;

  std::string cname = tc.class_name ();
  octave_stdout << "cname = " << cname << std::endl;

  // einfach mal alle "is"
  // egrep "bool is_[a-z_]*" libinterp/octave-value/ov.h | sed s/bool//g

#define DBG_OUT(x) cout << "  " << #x << " = " << tc.x() << endl;
   DBG_OUT(is_zero_by_zero)
   DBG_OUT(is_defined)
   DBG_OUT(is_undefined) 
   DBG_OUT(is_empty)
  DBG_OUT(is_cell)
  DBG_OUT(is_cellstr)
  DBG_OUT(is_real_scalar)
  DBG_OUT(is_real_matrix)
  DBG_OUT(is_complex_scalar)
  DBG_OUT(is_complex_matrix)
  DBG_OUT(is_bool_scalar)
  DBG_OUT(is_bool_matrix)
  DBG_OUT(is_char_matrix)
  DBG_OUT(is_diag_matrix)
  DBG_OUT(is_perm_matrix)
  DBG_OUT(is_string)
  DBG_OUT(is_sq_string)
  DBG_OUT(is_dq_string)
  DBG_OUT(is_range)
  DBG_OUT(is_map)
#if OCTAVE_MAJOR_VERSION == 4 && OCTAVE_MINOR_VERSION >= 2
  DBG_OUT(is_classdef_meta)
  DBG_OUT(is_classdef_superclass_ref)
  DBG_OUT(is_package)
  DBG_OUT(islogical)
#endif

  DBG_OUT(is_classdef_object)
  DBG_OUT(is_object)
  DBG_OUT(is_java)
  DBG_OUT(is_cs_list)
  DBG_OUT(is_magic_colon)
  DBG_OUT(is_null_value)
  DBG_OUT(is_double_type)
  DBG_OUT(is_single_type)
  DBG_OUT(is_float_type)
  DBG_OUT(is_int8_type)
  DBG_OUT(is_int16_type)
  DBG_OUT(is_int32_type)
  DBG_OUT(is_int64_type)
  DBG_OUT(is_uint8_type)
  DBG_OUT(is_uint16_type)
  DBG_OUT(is_uint32_type)
  DBG_OUT(is_uint64_type)
  DBG_OUT(is_integer_type)
  DBG_OUT(is_bool_type)
  DBG_OUT(is_real_type)
  DBG_OUT(is_complex_type)
  DBG_OUT(is_scalar_type)
  DBG_OUT(is_matrix_type)
  DBG_OUT(is_numeric_type)
  DBG_OUT(is_sparse_type)
  DBG_OUT(is_constant)
  DBG_OUT(is_function_handle)
  DBG_OUT(is_anonymous_function)
  DBG_OUT(is_inline_function)
  DBG_OUT(is_function)
  DBG_OUT(is_user_script)
  DBG_OUT(is_user_function)
  DBG_OUT(is_user_code)
  DBG_OUT(is_builtin_function)
  DBG_OUT(is_dld_function)
  DBG_OUT(is_mex_function)

if (! tc.is_cell ())
  DBG_OUT(is_true)





  //int32_t nr = tc.rows ();
  //int32_t nc = tc.columns ();

  if (tc.is_sparse_type ())
    {
      octave_stdout << "issparse, nnz = " <<  tc.nnz () << ", iscomplex = " << tc.is_complex_type () << std::endl;
      error ("JSON can't handle sparse matrix, convert to full matrix first (using 'full')");
    }
  else if (tc.is_inline_function ())
    {
      error ("Can't store inline function into JSON");
    }
  else if (tc.is_object ())
    {
      error ("Can't store object into JSON");
    }
  else if (tc.is_string ())
    {
      octave_stdout << "is_string()" << std::endl;

      // char matrix mit Leerzeichen speichern,
      // save_json(["foo"; "foobar"; "baz"])
      // ergibt ["foo   ","foobar","baz   "]

      //unwind_protect frame;

      charMatrix chm = tc.char_matrix_value ();

      octave_idx_type nrow = chm.rows ();
      //octave_idx_type ncol = chm.cols ();

      if (nrow > 1)
        writer.StartArray ();


      for (octave_idx_type i = 0; i < nrow; i++)
        {
          std::string tstr = chm.row_as_string (i);
          //const char *s = tstr.data ();

          octave_stdout << tstr << std::endl;
          
          writer.String (tstr.c_str ());
        }

      if (nrow > 1)
        writer.EndArray ();

    }
  else if (tc.is_range ())
    {
      octave_stdout << "is_range()" << std::endl;
      Range r = tc.range_value ();

      // FIXME: Should a range be stored separately
      // with base and inc?

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
      octave_stdout << "is_real_scalar() = " << tc.is_real_scalar() << std::endl;
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

      if (m.is_empty())
        writer.Null ();
      else
        {
          //octave_stdout << "numel=" << m.numel() << std::endl;
          //octave_stdout << "ndims=" << m.ndims() << std::endl;

          // swap first two dimensions, see also
          // https://stackoverflow.com/questions/45855978/agreement-how-a-matrix-2d-is-stored-as-json
          {
            Array<int> p(dim_vector(m.ndims(), 1));
            for (int k = 2; k < m.ndims(); ++k)
              p(k) = k;
            p(0) = 1;
            p(1) = 0;
            m = m.permute (p);
          }

          dim_vector d = m.dims ();

          // Achtung, hier schon gedreht
          //octave_stdout << "d = " << d.str () << std::endl;

          const double *pd = m.fortran_vec ();

          // Wieviel öffnende Klammern?
          for (int k = 0; k < d.length(); ++k)
            {
              writer.StartArray();
              //cout << "[" << endl;;
            }

          octave_idx_type *idx = new octave_idx_type[d.length()];
          std::fill_n (idx, d.length(), 0);

          for (int k = 0; k < m.numel(); ++k)
            {
              writer.Double (pd[k]);
              //cout << "pd[k]=" << pd[k] << endl;

              int r = d.increment_index (idx, 0);
              //cout << "k=" << k << ", r=" << r << endl;

              for (int i = 0; i < r; ++i)
                {
                  writer.EndArray();
                  //cout << "]" << endl;
                }

              if (r > 0 && r != d.length ())
                for (int i = 0; i < r; ++i)
                  {
                    writer.StartArray();
                    //cout << "[" << endl;
                  }

            }

          delete [] idx;
        }
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
  else if (tc.is_cell ())
    {
      octave_stdout << "is_cell()" << std::endl;
      Cell cell = tc.cell_value ();

      // swap first two dimensions, see also
      // https://stackoverflow.com/questions/45855978/agreement-how-a-matrix-2d-is-stored-as-json
      {
        Array<int> p(dim_vector(cell.ndims(), 1));
        for (int k = 2; k < cell.ndims(); ++k)
          p(k) = k;
        p(0) = 1;
        p(1) = 0;
        cell = cell.permute (p);
      }

      octave_stdout << "ndims() = " << cell.ndims () << std::endl;
      dim_vector d = cell.dims ();
      octave_stdout << "dims() = " << d.str() << std::endl;
      
      octave_idx_type *idx = new octave_idx_type[d.length()];
      std::fill_n (idx, d.length(), 0);
          
      int r = d.length ();
      if (d.is_vector ())
        r--;

      for (int i = 0; i < r; ++i)
        {
          //cout << "StartArray" << endl;
          writer.StartArray ();
        }
      do
        {
          octave_idx_type lidx = d.compute_index(idx);
          //cout << "cell recursive call" << endl;
          
          save_element (writer, cell(lidx));

          r = d.increment_index (idx, 0);
          //cout << "r = " << r << endl;

          for (int i = 0; i < r; ++i)
            {
              if (r == d.length ())
                i++;
              //cout << "EndArray" << endl;
              writer.EndArray ();
            }
          if (r != d.length () && r > 0)
            for (int i = 0; i < r; ++i)
              {
                //cout << "StartArray" << endl;
                writer.StartArray ();
              }

        }
      while (r != d.length ());
  
    }
  else if (tc.is_map ())
    {
      octave_map m = tc.map_value ();

      //char buf[64];
      //int32_t maxfieldnamelength = max_namelen + 1;

      octave_idx_type nfields = m.nfields ();
      octave_stdout << "isstruct(), nfields = " << nfields << std::endl;

      string_vector keys = m.keys ();

      // loop over keys
      for (octave_idx_type i = 0; i < nfields; i++)
        {
          std::string key = keys(i);
          octave_stdout << "keys(" << i << ") = " << key << std::endl;
        }

      octave_idx_type numel = m.numel ();
      octave_stdout << "m.numel = " << numel << std::endl;

      std::vector<const octave_value *> elts (nfields);
      for (octave_idx_type i = 0; i < nfields; i++)
        elts[i] = m.contents (keys(i)).data ();

      if (numel > 1)
        writer.StartArray();

      // loop over the elements
      for (octave_idx_type j = 0; j < numel; j++)
        {
          writer.StartObject();

          // Iterating over the list of keys will preserve the order
          // of the fields.
          for (octave_idx_type i = 0; i < nfields; i++)
            {
              //octave_stdout << elts[i][j] << std::endl;
              //octave_stdout << elts[i][j].double_value() << std::endl;
              octave_stdout << "class_name = " << elts[i][j].class_name() << std::endl;
              //octave_stdout << elts[i][j].matrix_value() << std::endl;

              writer.Key(keys(i).c_str ());

              //writer.Double(elts[i][j].double_value());

              // recursive call
              cout << "recursive call" << endl;
              save_element (writer, elts[i][j]);

            }

          writer.EndObject();
        }

      if (numel > 1)
        writer.EndArray();

      // FIXME: soll man einen struct als
      // { "a": [1,2,3], "b" : [10,20,30]}
      // oder
      // [{ "a": 1, "b": 10},{ "a": 2, "b": 20},{ "a": 3, "b": 30}]
      //
      // ich glaube letzteres ist wohl bei JSONiander üblicher...

    }
  else if (tc.is_integer_type())
    {
      cout << "tc.is_integer_type() = " << tc.is_integer_type() << endl;
    }
  else
    error ("not yet implemented");

  return true;
}

/*
%!test
%! a = [];
%! assert (load_json (save_json (a)), a);

%!test
%! a = pi;
%! assert (load_json (save_json (a)), a);

%!test
%! a = [2, 3, 4];
%! assert (load_json (save_json (a)), a);

%!test
%! a = [2; 3; 4];
%! assert (load_json (save_json (a)), a);

%!test
%! a = rand (4, 2, 3);
%! assert (load_json (save_json (a)), a, eps);

%!test
%! b(1).a = 4;
%! b(2).c = pi;
%! b(3).c = 2.718;
%! c = load_json (save_json (b));
%! assert ([c{:}], b, eps)
*/
