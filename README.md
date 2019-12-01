# octave-rapidjson, a JSON parser for GNU Octave >= 3.8

[GNU Octave](https://www.gnu.org/software/octave/) wrapper around [RapidJSON](http://rapidjson.org/)

These wrapper doesn't try to be compatible with jsondecode/jsonencode or JSONlab.

Instead it tries to map GNU Octave data structures as close as possible
to JSON and back. So the major design goal is that in GNU Octave

```
assert (load_json (save_json (foo)), foo)
```

works for almost any type of "foo".

## Function

### `load_json (json_string)`

Importing data from a JSON string tries to preserve the datatype whenever possible and if this doesn't slown down the import.

```
octave> x = load_json ('{"a":5, "b":3.14156}')
x =

  scalar structure containing the fields:

    a = 5
    b =  3.1416

octave> typeinfo (x.a)
ans = uint32 scalar
octave> typeinfo (x.b)
ans = scalar
```

N-dimensional matrices are returned as `matrix` (aka "double matrix")

```
octave> x = load_json ('[3,4,5]')
x =

   3   4   5

octave> typeinfo (x)
ans = matrix
```

As soon as one element in a JSON array can't mapped to a double value, a cell is returned:

```
octave> x = load_json ('[2, "foo", 3.14]')
x =
{
  [1,1] =  2
  [1,2] = foo
  [1,3] =  3.1400
}
```

#### load from file

 If you want to parse from a file use `load_json (fileread (filename))`


### `save_json (obj)`

* Saving a `matrix` always gives `double` values in JSON


Converting a struct array to JSON "unpacks" the struct array but I've been told by javascript programmers that this would be  
the expected behaviour...

```
octave> foo(1).a = 3;
octave> foo(1).b = pi;
octave> foo(2).a = 5;
octave> foo(2).b = 2.718;
octave> save_json (foo)
ans = [
    {
        "a": 3.0,
        "b": 3.141592653589793
    },
    {
        "a": 5.0,
        "b": 2.718
    }
]

```

If you want to read it back (and collapse the unpacked struct) you can do:

```

octave> load_json (ans)
ans =
{
  [1,1] =

    scalar structure containing the fields:

      a =  3
      b =  3.1416

  [1,2] =

    scalar structure containing the fields:

      a =  5
      b =  2.7180

}

octave> x = [ans{:}]
x =

  1x2 struct array containing the fields:

    a
    b
```

### Handling of NA, NaN, []

|GNU Octave| JSON |
|:--------:|:----:|
|NA        |"null"|
|NaN       |"NaN" |
|[]        |"[]"  |

```
octave:1> save_json (NA)
ans = null
octave:2> save_json ([])
ans = []
octave:3> save_json (NaN)
ans = NaN
octave:4> load_json ("null")
ans = NA
octave:4> load_json ("[]")
ans = [](0x0)
octave:5> load_json ("NaN")
ans = NaN
```

Using "NaN" in JSON is an extension and may not be comaptible with other libraries.

## Known problems / bugs / ToDo

## Side note

If you want to start using the C++ API liboctave here are some links to useful resources:

* [chapter "Oct-Files" in the GNU Octave manual](https://www.gnu.org/software/octave/doc/interpreter/Oct_002dFiles.html#Oct_002dFiles)
* [Doxygen generated documentation of liboctave](http://wiki.octave.org/Doxygen)
* [Manually generated class overview for liboctave on the wiki](http://wiki.octave.org/Project_liboctave_4.2)
* [C++ Tips and Tricks section on the wiki](http://wiki.octave.org/Tips_and_tricks#C.2B.2B) has a table with quivalent C++ code. Very usefull if you already know the m-file syntax
* [C++ code styleguide on the wiki](http://wiki.octave.org/C%2B%2B_style_guide)
* [Developer page on the wiki](http://wiki.octave.org/Developers)

## Other JSON Parser for GNU Octave on github

* https://github.com/fangq/jsonlab (pure .m files)
* https://github.com/gllmflndn/JSONio (uses C lib jsmn)
