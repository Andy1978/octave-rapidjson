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

#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/error/en.h"

#define INITIAL_ARRAY_SIZE 64
//#define DEBUG

#ifdef DEBUG
#define DBG_MSG2(d, a, b) DBS(d);\
                   std::cout << __FILE__ << ":"\
                             << __FUNCTION__ << ":"\
                             << __LINE__ << " "\
                             << a << " "\
                             << b << std::endl
#define DBG_MSG1(d, a) DBG_MSG2(d, a, "")
#define DBS(d) for (int __k__ = 0; __k__ < d; ++__k__) std::cout << "-";
#else
#define DBG_MSG2(d, a, b)
#define DBG_MSG1(d, a)
#define DBS(d)
#endif


class parse_state
{
public:

  parse_state (int depth)
    : _depth (depth),
      is_array (0),
      array (dim_vector (1, INITIAL_ARRAY_SIZE))
  {
    DBG_MSG2(_depth, "constructor", _depth);
  }

  ~parse_state ()
  {
    DBG_MSG1(_depth, "destructor");
  }

  octave_scalar_map result;

  void push_back (octave_value v)
  {
    DBG_MSG1(_depth, "");

    if (is_array)
      {
#if OCTAVE_MAJOR_VERSION == 4 && OCTAVE_MINOR_VERSION >0 || OCTAVE_MAJOR_VERSION > 4
        if (! v.isnumeric ())
#else
        if (! v.is_numeric_type ())
#endif
          array_is_numeric = false;

        assert (array.rows () == 1);
        unsigned c = array.columns ();
        DBG_MSG2(_depth, "array.columns () = ", c);

        if (array_items == c)
          array.resize (dim_vector (1, 2 * c));

        array(0, array_items++) = v;
      }
    else
      {
        result.contents (_key) = v;
      }
  }

  void key (const char* str)
  {
    DBG_MSG1(_depth, str);
    _key = str;
  }

  void start_object ()
  {
    DBG_MSG1(_depth, "");
    array_is_numeric = false;
  }

  void start_array ()
  {
    DBG_MSG1(_depth, "");
    is_array = true;
    array_is_numeric = true;
    array_items = 0;
  }

  void end_array ()
  {
    DBG_MSG2(_depth, "array_items = ", array_items);
    //assert (elementCount == ps.back().array_items);
    is_array = false;

    // return Matrix if the array didn't contains string
    if (array_is_numeric)
      {
        DBG_MSG1(_depth, "array_is_numeric");
        RowVector tmp(array_items);
        for (unsigned k = 0; k < array_items; ++k)
          tmp (k) = array(k).double_value();
        result.contents (_key) = tmp;
      }
    else
      {
        // FIMXE: looks like there is no "extract" for Cells?
        array.resize (dim_vector (array.rows(), array_items));
        result.contents (_key) = array;
      }
  }

private:
  int _depth;
  std::string _key;
  bool is_array;
  bool array_is_numeric;
  unsigned array_items;
  Cell array;
};

class MyHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, MyHandler>
{
public:

  octave_scalar_map result;
  std::vector <parse_state*> ps;

public:

  //MyHandler ()

  bool Null()
  {
    DBG_MSG1 (0, "");
    ps.back()->push_back (Matrix());
    return true;
  }

  bool Bool(bool b)
  {
    DBG_MSG1 (0, b);
    ps.back()->push_back ((b)? true: false);
    return true;
  }

  bool Uint(unsigned u)
  {
    DBG_MSG1 (0, u);
    ps.back()->push_back (octave_uint32 (u));
    return true;
  }

  bool Int(signed i)
  {
    DBG_MSG1 (0, i);
    ps.back()->push_back (octave_int32 (i));
    return true;
  }

  bool Int64(int64_t i)
  {
    DBG_MSG1 (0, i);
    ps.back()->push_back (octave_int64 (i));
    return true;
  }
  bool Uint64(uint64_t u)
  {
    DBG_MSG1 (0, u);
    ps.back()->push_back (octave_uint64(u));
    return true;
  }

  bool Double(double d)
  {
    DBG_MSG1 (0, d);
    ps.back()->push_back (d);
    return true;
  }

  bool String(const char* str, rapidjson::SizeType length, bool copy)
  {
    (void) length;
    (void) copy;
    DBG_MSG2 (0, str, length);
    ps.back()->push_back (str);
    return true;
  }

  bool StartObject()
  {
    DBG_MSG1 (0, "");
    int d = ps.size() + 1;
    ps.push_back(new parse_state(d));
    ps.back()->start_object ();
    return true;
  }
  bool Key(const char* str, rapidjson::SizeType length, bool copy)
  {
    (void) copy;
    if (strlen (str) != length)
      error ("octave-rapidjson can't handle strings with zeros");

    ps.back()->key(str);
    return true;
  }
  bool EndObject(rapidjson::SizeType memberCount)
  {
    (void) memberCount;
    int n = ps.size ();
    DBG_MSG2 (0, "ps.size() = ", n);

    if (n > 1)
      ps[n-2]->push_back (ps[n-1]->result);
    else
      result = ps[n-1]->result;

    delete ps.back ();
    ps.pop_back ();

    return true;
  }
  bool StartArray()
  {
    ps.back()->start_array ();
    return true;
  }

  bool EndArray(rapidjson::SizeType elementCount)
  {
    (void) elementCount;
    ps.back()->end_array ();
    return true;
  }
};


DEFUN_DLD (load_json, args,, "load_json (json_str)")
{
  if (args.length () != 1)
    print_usage ();

  MyHandler handler;
  rapidjson::Reader reader;
  std::string json = args(0).string_value ();
  DBG_MSG2(0, "json = ", json);

  rapidjson::StringStream ss(json.c_str());
  rapidjson::ParseResult ok = reader.Parse(ss, handler);
  if (! ok)
    {
      error ("JSON parse error: '%s' at offset %u",
             GetParseError_En (ok.Code()),
             ok.Offset());
    }
  //~ Matrix a(dim_vector(3,2));
  //~ for (int k=0; k<a.numel();++k)
  //~ a.fortran_vec()[k] = k;
  //~ std::cout << a << std::endl;
  //~ std::cout << a.extract(0,1,2,1) << std::endl;

  return ovl (handler.result);
}

/*
%!test
%! json = '{ "hello" : "world", "t" : true , "f" : false, "n": null, "i":-123, "u":456, "pi": 3.1416, "li": -765432986, "a":[1, 2, 3, 4], "b": ["foo", 4] } ';
%! r = load_json (json);
%! assert (r.hello, "world")
%! assert (r.t, true)
%! assert (r.f, false)
%! assert (r.n, [])
%! assert (r.i, int32 (-123))
%! assert (r.u, uint32 (456))
%! assert (r.pi, pi, 1e-5)
%! assert (r.a, [1 2 3 4])
%! assert (r.b, {"foo", 4})

%!xtest
%! disp ("Matrix not yet supported")
%! json = '{ "a": [[1,2],[3,4]]}';
%! r =load_json (json);
%! assert (r.a, [1 2; 3 4]);
*/
