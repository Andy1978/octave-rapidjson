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

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/en.h"

//#define DEBUG
#include "debug.h"

// starting with 4.4, the is_* style methods were deprecated
// see also https://sourceforge.net/p/octave/image/ci/default/tree/src/octave-wrappers.h

#if (OCTAVE_MAJOR_VERSION == 4 && OCTAVE_MINOR_VERSION < 4) || OCTAVE_MAJOR_VERSION < 4
#define IS_BOOL tc.is_bool_type ()
#define IS_EMPTY(x) x.is_empty ()
#define IS_SPARSE(x) x.is_sparse_type ()
#define IS_OBJECT(x) x.is_object ()
#define IS_INT(x) x.is_integer_type ()
#define IS_REAL(x) x.is_real_type ()
#define IS_COMPLEX(x) x.is_complex_type ()
#define IS_NA(x) octave::math::is_NA (x)
#define IS_STRUCT(x) x.is_map ()
#define IS_CELL(x) x.is_cell ()
#else
#define IS_BOOL tc.islogical ()
#define IS_EMPTY(x) x.isempty ()
#define IS_SPARSE(x) x.issparse ()
#define IS_OBJECT(x) x.isobject ()
#define IS_INT(x) x.isinteger ()
#define IS_REAL(x) x.isreal ()
#define IS_COMPLEX(x) x.iscomplex ()
#define IS_NA(x) octave::math::isna (x)
#define IS_STRUCT(x) x.isstruct ()
#define IS_CELL(x) x.iscell ()
#endif

using namespace rapidjson;
using namespace std;

template <typename T> void save_matrix (PrettyWriter<StringBuffer, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag> &writer, T);
bool save_element (PrettyWriter<StringBuffer, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag> &writer, const octave_value& tc);

DEFUN_DLD (save_json, args,, "save_json (obj)")
{
  int nargs = args.length ();

  if (nargs != 1)
    print_usage ();

  StringBuffer s;
  PrettyWriter<StringBuffer, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag> writer(s);
  save_element (writer, args(0));

  return ovl (s.GetString());
}

template <typename T> void save_matrix (PrettyWriter<StringBuffer, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag> &writer, T m)
{
  DBG_OUT(m.numel ());
  DBG_OUT(m.ndims ());

  // T can be NDArray or Cell
  if (IS_EMPTY(m))
    {
      writer.StartArray ();
      writer.EndArray ();
    }
  else
    {

      // swap first two dimensions, see also
      // https://stackoverflow.com/questions/45855978/agreement-how-a-matrix-2d-is-stored-as-json
      {
        // FIXME: I'm sure there must be a better way to create p
        Array<int> p(dim_vector(m.ndims(), 1));
        for (int k = 2; k < m.ndims(); ++k)
          p(k) = k;
        p(0) = 1;
        p(1) = 0;
        m = m.permute (p);
      }

      // Dimensions of input (after swapping the first two dimensions)
      dim_vector d = m.dims ();
      DBG_OUT(m.dims ().str());

      int ndims = d.length ();
      octave_idx_type *idx = new octave_idx_type[ndims];
      std::fill_n (idx, ndims, 0);

      int is_row_vector = (ndims == 2 && d(1) == 1);

      int dim_index = 0;

      // dive into
      for (int i = 0; i < (ndims - is_row_vector); ++i)
        {
          DBG_MSG1(0, "StartArray");
          writer.StartArray ();
        }

      do
        {
          octave_idx_type linear_idx = d.compute_index (idx);

          // FIXME: hier prüfen, ob es sich um eine Nicht-Cell Matrix handelt,
          // wenn ja, dann kann man sich den rekursiven Aufruf sparen und
          // ggf. mehr Geschwindigkeit rausholen

          //cout << "cell recursive call" << endl;
          save_element (writer, m(linear_idx));

          dim_index = d.increment_index (idx, 0);
          DBG_OUT (dim_index)

          for (int i = 0; i < dim_index; ++i)
            {
              // omit last EndArray if it's a row vector
              if (dim_index == ndims)
                i += is_row_vector;

              DBG_MSG1(0, "EndArray");
              writer.EndArray ();
            }

          // reopen array if we haven't reached the end
          if (dim_index > 0 && dim_index != ndims)
            for (int i = 0; i < dim_index; ++i)
              {
                DBG_MSG1(0, "StartArray");
                writer.StartArray ();
              }

        }
      while (dim_index != ndims);



      delete [] idx;
    }

}

bool save_element (PrettyWriter<StringBuffer, UTF8<>, UTF8<>, CrtAllocator, kWriteNanAndInfFlag> &writer, const octave_value& tc)
{
#ifdef DEBUG
  std::cout << "-------------- save_element --------------------" << std::endl;
#endif

  std::string cname = tc.class_name ();
  DBG_OUT(cname);

  // einfach mal alle "is"
  // egrep "bool is_[a-z_]*" libinterp/octave-value/ov.h | sed s/bool//g

#if 0
  DBG_OUT(tc.is_zero_by_zero());
  DBG_OUT(tc.is_defined());
  DBG_OUT(tc.is_undefined());
  DBG_OUT(tc.is_empty());
  DBG_OUT(tc.is_cell());
  DBG_OUT(tc.is_cellstr());
  DBG_OUT(tc.is_real_scalar());
  DBG_OUT(tc.is_real_matrix());
  DBG_OUT(tc.is_complex_scalar());
  DBG_OUT(tc.is_complex_matrix());
  DBG_OUT(tc.is_bool_scalar());
  DBG_OUT(tc.is_bool_matrix());
  DBG_OUT(tc.is_char_matrix());
  DBG_OUT(tc.is_diag_matrix());
  DBG_OUT(tc.is_perm_matrix());
  DBG_OUT(tc.is_string());
  DBG_OUT(tc.is_sq_string());
  DBG_OUT(tc.is_dq_string());
  DBG_OUT(tc.is_range());
  DBG_OUT(tc.is_map());
#if OCTAVE_MAJOR_VERSION == 4 && OCTAVE_MINOR_VERSION > 2
  DBG_OUT(tc.is_classdef_meta());
  DBG_OUT(tc.is_classdef_superclass_ref());
  DBG_OUT(tc.is_package());
  DBG_OUT(tc.islogical());
#endif

  DBG_OUT(tc.is_classdef_object());
  DBG_OUT(tc.is_object());
  DBG_OUT(tc.is_java());
  DBG_OUT(tc.is_cs_list());
  DBG_OUT(tc.is_magic_colon());
  DBG_OUT(tc.is_null_value());
  DBG_OUT(tc.is_double_type());
  DBG_OUT(tc.is_single_type());
  DBG_OUT(tc.is_float_type());
  DBG_OUT(tc.is_int8_type());
  DBG_OUT(tc.is_int16_type());
  DBG_OUT(tc.is_int32_type());
  DBG_OUT(tc.is_int64_type());
  DBG_OUT(tc.is_uint8_type());
  DBG_OUT(tc.is_uint16_type());
  DBG_OUT(tc.is_uint32_type());
  DBG_OUT(tc.is_uint64_type());
  DBG_OUT(tc.is_integer_type());
  DBG_OUT(tc.is_bool_type());
  DBG_OUT(tc.is_real_type());
  DBG_OUT(tc.is_complex_type());
  DBG_OUT(tc.is_scalar_type());
  DBG_OUT(tc.is_matrix_type());
  DBG_OUT(tc.is_numeric_type());
  DBG_OUT(tc.is_sparse_type());
  DBG_OUT(tc.is_constant());
  DBG_OUT(tc.is_function_handle());
  DBG_OUT(tc.is_anonymous_function());
  DBG_OUT(tc.is_inline_function());
  DBG_OUT(tc.is_function());
  DBG_OUT(tc.is_user_script());
  DBG_OUT(tc.is_user_function());
  DBG_OUT(tc.is_user_code());
  DBG_OUT(tc.is_builtin_function());
  DBG_OUT(tc.is_dld_function());
  DBG_OUT(tc.is_mex_function());

  //~ if (! tc.is_cell ())
  //~ {
  //~ DBG_OUT(tc.is_true());  // not defined for cell
  //~ // FIXME: if tc is a double and NaN, this method shows an error:
  //~ // invalid conversion from NaN to logical
  //~ }

#endif

  if (IS_SPARSE(tc))
    {
#ifdef DEBUG
      std::cout << "issparse, nnz = " <<  tc.nnz () << ", iscomplex = " << tc.iscomplex () << std::endl;
#endif
      error ("JSON can't handle sparse matrix, convert to full matrix first (using 'full')");
    }
  else if (tc.is_inline_function ())
    {
      error ("Can't store inline function into JSON");
    }
  else if (IS_OBJECT(tc))
    {
      error ("Can't store object into JSON");
    }
  else if (tc.is_string ())
    {
      DBG_MSG1(0, "");
      if (IS_EMPTY(tc))
        writer.String ("");
      else
        {
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

              DBG_OUT(chm.row_as_string(i));

              writer.String (tstr.c_str ());
            }

          if (nrow > 1)
            writer.EndArray ();
        }
    }
  else if (tc.is_range ())
    {
      DBG_MSG1(0, "");
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
  else if (tc.is_bool_scalar ())
    {
      DBG_MSG1(0, "");
      writer.Bool (tc.bool_value ());
    }
  else if (tc.is_bool_matrix ())
    {
      DBG_MSG1(0, "");
      boolNDArray b = tc.bool_array_value ();
      save_matrix (writer, b);
    }
  else if (tc.is_real_scalar ())
    {
      DBG_MSG1(0, "");
      if (IS_INT(tc))
        writer.Int64(tc.int64_value ());
      else if (IS_REAL(tc))
        {
          double val = tc.double_value ();
          if (IS_NA(val))
            writer.Null ();
          else
            writer.Double (val);
        }
      else if (IS_COMPLEX(tc))
        error ("complex not yet supported");
    }
  else if (tc.is_matrix_type ())
    {
      DBG_MSG1(0, "");
      if (IS_INT(tc))
        save_matrix (writer, tc.int64_array_value ());
      else
        save_matrix (writer, tc.array_value ());
    }
  else if (IS_CELL(tc))
    {
      DBG_MSG1(0, "");
      Cell cell = tc.cell_value ();
      save_matrix (writer, cell);
    }
  else if (tc.is_complex_scalar ())
    {
      DBG_MSG1(0, "");
      //Complex tmp = tc.complex_value ();
      //~ os.write (reinterpret_cast<char *> (&tmp), 16);
    }
  else if (tc.is_complex_matrix ())
    {
      DBG_MSG1(0, "");
      ComplexMatrix m_cmplx = tc.complex_matrix_value ();
      //~ Matrix m = ::real (m_cmplx);
      //~ std::streamsize n_bytes = 8 * static_cast<std::streamsize> (len);
      //~ os.write (reinterpret_cast<const char *> (m.data ()), n_bytes);
      //~ m = ::imag (m_cmplx);
      //~ os.write (reinterpret_cast<const char *> (m.data ()), n_bytes);
    }
  else if (IS_STRUCT(tc))
    {
      DBG_MSG1(0, "");
      octave_map m = tc.map_value ();

      //char buf[64];
      //int32_t maxfieldnamelength = max_namelen + 1;

      octave_idx_type nfields = m.nfields ();
      DBG_OUT (nfields);

      string_vector keys = m.keys ();

      // loop over keys
      for (octave_idx_type i = 0; i < nfields; i++)
        {
          std::string key = keys(i);
          DBG_OUT(keys(i));
        }

      octave_idx_type numel = m.numel ();
      DBG_OUT(m.numel ());

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

              DBG_OUT(elts[i][j].class_name());

              writer.Key(keys(i).c_str ());

              // recursive call
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
  else if (IS_INT(tc))
    {
      DBG_MSG1(0, "");
      DBG_OUT(IS_INT(tc));
    }
  else
    error ("not yet implemented");

  return true;
}

/*

*** empty ***

%!test
%! a = [];
%! assert (load_json (save_json (a)), a);

%!test
%! a.foo = "";
%! assert (load_json (save_json (a)), a);

*** logical ***

%!test
%! a.foo = true;
%! assert (load_json (save_json (a)), a);

%!test
%! a.foo = false;
%! assert (load_json (save_json (a)), a);

%!test
%! a = [true false];
%! assert (save_json (a), "[\n    true,\n    false\n]");

*** scalars ***

%!test
%! a = pi;
%! assert (load_json (save_json (a)), a);

*** row vector ***

%!test
%! a = [2, 3, 4];
%! assert (load_json (save_json (a)), a);

*** column vector ***

%!test
%! a = [2; 3; 4];
%! assert (load_json (save_json (a)), a);

*** Matrix ***

%!test
%! a = rand (4, 2, 3);
%! assert (load_json (save_json (a)), a, eps);

*** NDArray ***

%!test
%! a = rand (4, 3);
%! assert (load_json (save_json (a)), a, eps);

*** strings ***

%!test
%! a.foo = "bar";
%! assert (load_json (save_json (a)), a)

%!test
%! a.foo = 'bar';
%! assert (load_json (save_json (a)), a)

%!test
%! a = {"foo", "bar", "hello"};
%! assert (load_json (save_json (a)), a)

%!test
%! a = {"foo"; "bar"; "hello"};
%! assert (load_json (save_json (a)), a)

*** structs ***

%!test
%! b(1).a = 4;
%! b(2).c = pi;
%! b(3).c = 2.718;
%! c = load_json (save_json (b));
%! assert ([c{:}], b, eps)

*** NaN, Inf, -Inf (not standard JSON) ***

%!test
%! x = [NaN, Inf, -Inf];
%! c = load_json (save_json (x));
%! assert (c, x);

*** mapping Octave <-> JSON ***
***         NA     <-> null ***
***         []     <-> []   ***

%!test
%! x.foo = NA;
%! x.bar = [];
%! c = load_json (save_json (x));
%! assert (c, x);

%!function out = rm_ws (in)
%!  ## remove whitespace
%!  out = regexprep (save_json (in), "[[:space:]]", "");
%!endfunction

*** integers ***

%!test
%! a = int8([-50 1 5 50]);
%! assert (rm_ws (a), "[-50,1,5,50]");

%!test
%! a = uint8([1 5 50]);
%! assert (rm_ws (a), "[1,5,50]");

%!test
%! a = uint16([0 5 2^16-1]);
%! assert (rm_ws (a), "[0,5,65535]");

%!test
%! a = int16([0 5 10; 45 55 65]);
%! assert (rm_ws (a), "[[0,5,10],[45,55,65]]");

%!test
%! a = uint64([4500139062]);
%! assert (rm_ws (a), "4500139062");

%!test
%! a = int64([4500139062, -4500139062]);
%! assert (rm_ws (a), "[4500139062,-4500139062]");

*/
