function compare_with_jsonlab (json, ignore_assert = 0)

  addpath ("~/src/jsonlab/");

  num = 100;
  t1 = t2 = zeros (num, 1);

  for k=1:num
    tic
    a = load_json(json);
    t1(k) = toc;
    tic
    b = loadjson(json);
    t2(k) = toc;
    if (! ignore_assert)
      assert (a, b)
    endif
  endfor
  
  printf ("t2/t1: mean = %.1f\n", mean (t2)/mean(t1));

endfunction
