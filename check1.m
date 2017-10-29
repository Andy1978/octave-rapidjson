addpath ("~/src/jsonlab/");

## works
#json = ' { "hello" : "world", "t" : true , "f" : false, "n": null, "i":-123, "u":456, "pi": 3.1416, "li": -765432986, "a":[1, 2, 3, 4], "b": ["foo", 4] } ';
#json = '{ "a" : 5, "b": {"c":4, "d":5} } ';

json = '{ "b" : [[1,2],[3,4]] } ';

tic
a = load_json2(json)
toc

tic
b = loadjson(json);
toc

assert (a, b)
#structfun (@typeinfo, a, "uniformOutput", false)
#structfun (@typeinfo, b, "uniformOutput", false)


