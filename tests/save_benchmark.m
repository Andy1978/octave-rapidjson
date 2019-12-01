# compare with jsonlab

addpath ("../")
addpath ("~/src/jsonlab/");

json = load_json (fileread ("big1.json"));

tic
x = savejson ("", json);
t1 = toc
numel (x)

tic
y = save_json (json);
t2 = toc
numel (y)

printf ("speedup factor %.1f\n", t1/t2)
