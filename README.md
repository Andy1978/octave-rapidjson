# octave-rapidjson

[GNU Octave](https://www.gnu.org/software/octave/) wrapper around [RapidJSON](http://rapidjson.org/)

## Ready for test

* `load_json (json_string)`. If you want to parse from file use `load_json (fileread (filename))`

## ToDo

* Add many, many tests and "make check" target to Makefile
* Implement 2D arrays (Matrix) and later N-dimesnional arrays
* Implement `save_json (json_string)`

## Optimization ideas

* Start reading a JSON array to a Octave Matrix (not Cell as it's currently done).
  As soon as the first non-numerical value is encountered, copy the already
  read data into a Cell and keep processing. I think this may be a better approach
  than the current used (store all into cell and convert afterwards to Matrix if values are numeric)
  because the usual user might store numerical matrices and not mixed ones.
