addpath ("~/src/jsonlab/");

fn = "Plan1.json_1495619229.json"
tic
a = load_json(fileread(fn));
toc
tic
b = loadjson(fn);
toc

assert (a, b)
