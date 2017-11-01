addpath ("../")
addpath ("~/src/jsonlab/");

json = ' { "hello" : "world", "t" : true , "f" : false, "n": null, "i":-123, "u":456, "pi": 3.1416, "li": -765432986, "a":[1, 2, 3, 4], "b": ["foo", 4] } ';
compare_with_jsonlab (json);

# rapidjson returns Cell for 3 dimensional matrix. Is this a bug?
json = '{ "a" : [[[1,2],[3,4]],[[5,6],[7,8]]], "b" : [10,20], "c": [100,200] }';
compare_with_jsonlab (json, 1);

json = '{ "a" : 5, "b": {"c":4, "d":5} } ';
compare_with_jsonlab (json);

json = '{ "b" : [[1,2],[3,4],[5,6]] } ';
compare_with_jsonlab (json);

# Not numeric entry. I think this implementation acts better here
json = '{ "b" : [[1,2],[5,"bar"]]}';
compare_with_jsonlab (json, 1);

json = '[3,4,5]';
compare_with_jsonlab (json, 0);

json = fileread ("medium1.json");
compare_with_jsonlab (json, 0, 1);
