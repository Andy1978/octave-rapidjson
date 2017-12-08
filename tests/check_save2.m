more off

# beliebig komplexe strukturen aufbauen
clear all

#x.a = rand(2,3,4);
#x.b = uint32 (randi (10, 3, 2))
#x.c = true;
#x.d = false;
#save_json (x, 1)

%~ a.foo = "bar"
%~ a.foox = "booo"
%~ a.fookk = rand(2,2)
%~ save_json (a)

load ("json_prg");
json_prg = rmfield (json_prg, "cplan")
json_prg = rmfield (json_prg, "steps_and_params")
tic
a = save_json (json_prg);
toc

addpath ("~/src/jsonlab")
tic
b = savejson ("", json_prg);
toc
