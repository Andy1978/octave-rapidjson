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

DEFUN_DLD (save_json, args,, "save_json (obj)")
{
  if (args.length () != 1)
    print_usage ();

// I think ls-mat5.cc:save_mat5_binary_element
// and the other save functions are a good source to see
// how to traverse an octave-value

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
}
