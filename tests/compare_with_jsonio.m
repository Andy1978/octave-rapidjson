## Comparison with https://github.com/gllmflndn/JSONio

# git clone https://github.com/gllmflndn/JSONio.git
# cd JSONio
# rm *.mex*
# mkoctfile --mex jsonread.c jsmn.c

addpath ("./JSONio");
addpath ("../");

fn = "big1.json";

num = 100;
t1 = t2 = zeros (num, 1);

for k=1:num
  tic
  a = load_json (fileread(fn));
  t1(k) = toc;
  tic
  b = jsonread(fn);
  t2(k) = toc;
endfor

printf ("t2/t1: mean = %.1f\n", mean (t2)/mean(t1));
