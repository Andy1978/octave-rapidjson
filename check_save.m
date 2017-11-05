
%~ a = rand(3,2)
%~ foo (a);

%~ a = "hello"
%~ foo (a); 

%~ a = ["hello";"bar"]
%~ foo (a);

%~ a = 0:0.2:0.7
%~ foo (a);

%~ clear a
%~ a.b = 6;
%~ a.c = 7
%~ foo (a);

clear b
b(1).a = 4
b(2).c = pi
b(3).c = 2.718
foo(b)
