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

#include <vector>
#include "dynContainer.h"

#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"

//#define DEBUG
#include "debug.h"

class parse_state
{
public:

  parse_state (int depth)
    : _depth (depth),
      is_json_array (0),
      array (NULL)
  {
    DBG_MSG2(_depth, "constructor, _depth = ", _depth);
  }

  ~parse_state ()
  {
    DBG_MSG2(_depth, "destructor, _depth = ", _depth);
    delete (array);
  }

  octave_scalar_map result;

  void push_back (octave_value v)
  {
    DBG_MSG2(_depth, "is_json_array=", is_json_array);

    if (is_json_array)
      {
        array->append_value (v);
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

  void start_array ()
  {
    DBG_MSG1(_depth, "");
    is_json_array = true;

    if (! array)
      array = new dynContainer;
    array->ob ();
  }

  void end_array ()
  {
    DBG_MSG1(_depth, "");
    is_json_array = false;

    array->cb ();

    if (array->get_depth () == 0)
      {
        result.contents (_key) = array->get_array ();
        delete (array);
        array = NULL;
      }
  }

private:
  int _depth;             // only for debug output
  std::string _key;
  bool is_json_array;     // we are between start_array() and end_array()

  dynContainer *array;
};

class JSON_Handler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, JSON_Handler>
{
private:

  std::vector <parse_state*> ps;

public:

  JSON_Handler ()
  {
    DBG_MSG1 (0, "constructor");
    ps.push_back (new parse_state(0));
    ps.back()->key("root");
  }

  ~JSON_Handler ()
  {
    DBG_MSG1 (0, "destructor");
    for (unsigned k = 0; k < ps.size(); ++k)
      delete (ps[k]);
  }

  octave_value result ()
  {
    int n = ps.size ();
    if (n != 1)
      warning ("result () called too early, parsing is still in progress");

    //return ps[0]->result; // FIXME: eigentlich wollte ich hier nur "root" zurück geben
    //.contents ("root").scalar_map_value();

    octave_value tmp = ps[0]->result.contents ("root");
    return tmp;

  }

  bool Null()
  {
    DBG_MSG1 (0, "");
    ps.back()->push_back (lo_ieee_na_value ());
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
    int d = ps.size () + 1;
    ps.push_back (new parse_state(d));
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
    //else
    //  result = ps[n-1]->result;

    delete ps.back ();  // this IS ps[n-1]
    ps.pop_back ();

    return true;
  }

  bool StartArray()
  {
    // Sollte JSON mit '[' anfangen, z.B.
    // '[{"a":5}]'
    // oder
    // '[2,3]'

    //if (! ps.size())
    //  ps.push_back (new parse_state(ps.size () + 1));

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

  JSON_Handler handler;
  rapidjson::Reader reader;
  std::string json = args(0).string_value ();
  DBG_MSG2(0, "json = ", json);

  rapidjson::StringStream ss(json.c_str());
  rapidjson::ParseResult ok = reader.Parse<rapidjson::kParseNanAndInfFlag>(ss, handler);

  //rapidjson::Document d;
  //rapidjson::ParseResult ok = d.ParseStream(ss);

  if (! ok)
    {
      // show substring with error
#ifdef DEBUG
#define HALF_DBG_OUT 10
      int left = HALF_DBG_OUT;
      if (int(ok.Offset()) - left < 0)
        left = ok.Offset();

      // FIXME: this writes to stdout even when running "test load_json"
      // -> find a way to handle this better
      std::cout << "JSON Debug:" << json.substr (ok.Offset() - left, left + HALF_DBG_OUT + 1) << endl;
      std::cout << "JSON Debug:";
      for (int k = 0; k < left; ++k)
        std::cout << " ";
      std::cout << "^" << endl;;
#endif
      error ("JSON parse error: '%s' at offset %zu",
             rapidjson::GetParseError_En (ok.Code()),
             ok.Offset());
    }

  return ovl (handler.result ());
}

/*
%!test
%! json = '{ "hello" : "world", "t" : true , "f" : false, "n": null, "i":-123, "u":456, "pi": 3.1416, "li": -765432986, "a":[1, 2, 3, 4], "b": ["foo", 4] } ';
%! r = load_json (json);
%! assert (r.hello, "world")
%! assert (r.t, true)
%! assert (r.f, false)
%! assert (r.n, NA)
%! assert (r.i, int32 (-123))
%! assert (r.u, uint32 (456))
%! assert (r.pi, pi, 1e-5)
%! assert (r.a, [1 2 3 4])
%! assert (r.b, {"foo", 4})

%!test
%! json = '{ "a": [[1,2],[3,4]]}';
%! r = load_json (json);
%! assert (r.a, [1 2; 3 4], eps);

%!test
%! json = '{ "a" : [[[1,2],[3,4]],[[5,6],[7,8.1]]], "b" : [10,20], "c": [100,200] }';
%! r = load_json (json);
%! assert (r.a, cat (3, [1 2; 3 4], [5 6; 7 8.1]), eps);
%! assert (r.b, [10 20], eps);
%! assert (r.c, [100 200], eps);

%!test
%! json = '[2,3,4]';
%! r = load_json (json);
%! assert (r, 2:4, eps)

%!test
%! json = '[[2.1],[3.4],[1.6]]';
%! r = load_json (json);
%! assert (r, [2.1, 3.4, 1.6].', eps)

# NaN/Inf is a JSON extension (not in the standard)
%!test
%! assert (load_json ("[NaN, Inf, -Inf]"), [NaN, Inf, -Inf])

%!error <'Invalid value.' at offset 5> load_json ('[1,2,,3]');
%!error <'Missing a colon after a name of object member.' at offset 5> load_json ('{"a" 5}');

*/
