# octave-rapidjson, a JSON parser for GNU Octave

[GNU Octave](https://www.gnu.org/software/octave/) wrapper around [RapidJSON](http://rapidjson.org/)

## Ready for test

* `load_json (json_string)`. If you want to parse from file use `load_json (fileread (filename))`

## ToDo

* ~~Add many, many tests and "make check" target to Makefile~~
* ~~Implement 2D arrays (Matrix) and later N-dimensional arrays~~
* Implement `save_json (json_string)`

## Optimization ideas

* ~~Start reading a JSON array to a Octave Matrix (not Cell as it's currently done).
  As soon as the first non-numerical value is encountered, copy the already
  read data into a Cell and keep processing. I think this may be a better approach
  than the current used (store all into cell and convert afterwards to Matrix if values are numeric)
  because the usual user might store numerical matrices and not mixed ones.~~ finished with 7a4156a4da8751931d5e76c65e115e75808065fb

## Side note

If you want to start using the C++ API liboctave here are some links to useful resources:

* [chapter "Oct-Files" in the GNU Octave manual](https://www.gnu.org/software/octave/doc/interpreter/Oct_002dFiles.html#Oct_002dFiles)
* [Doxygen generated documentation of liboctave](http://wiki.octave.org/Doxygen)
* [Manually generated class overview for liboctave on the wiki](http://wiki.octave.org/Project_liboctave_4.2)
* [C++ Tips and Tricks section on the wiki](http://wiki.octave.org/Tips_and_tricks#C.2B.2B) has a table with quivalent C++ code. Very usefull if you already know the m-file syntax
* [C++ code styleguide on the wiki](http://wiki.octave.org/C%2B%2B_style_guide)
* [Developer page on the wiki](http://wiki.octave.org/Developers)

## Other JSON Parser for GNU Octave on github

* https://github.com/fangq/jsonlab (pure .m files
* https://github.com/gllmflndn/JSONio (uses C lib jsmn)
