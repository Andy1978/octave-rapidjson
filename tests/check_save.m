more off

%~ a = rand(3,2)
%~ save_json (a);

%~ a = "hello"
%~ save_json (a); 

%~ a = ["hello";"bar"]
%~ save_json (a);

%~ a = 0:0.2:0.7
%~ save_json (a);

####################################
%~ clear a
%~ a.b = 6;
%~ a.c = 7
%~ save_json (a)


b(1).a = 4;
b(2).c = pi;
b(3).c = 2.718;
save_json (b)

%~ clear b
%~ b(1).a = 1;
%~ b(1).c = 2;

%~ b(2).a = 3;
%~ b(2).c = 4
%~ save_json(b)

#a = reshape (1:6, 1,2,3)

#a = reshape (1:8, 2,2,2)

#a = [1;2;3];
#a = [1 2; 3 4]

#a = reshape (1:6, 2,3)

#x = save_json (a)

#r = "[[[1,2]],[[3,4]],[[5,6]]]";

#####################################
## real_matrix

if 1
  for k=1:100

    ndim = randi (5, 1, 1);
    a = rand(randi (6, ndim, 1));
    assert (load_json (save_json (a)), a, eps)

  endfor
endif

####################################
## integer
if 1
  a = uint32(5)
  assert (load_json (save_json (a)), a)
endif
