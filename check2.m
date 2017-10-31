addpath ("~/src/jsonlab/");

#json = urlread ("https://next.json-generator.com/api/json/get/4yozvx-0m");

if (exist ("tmp", "file"))
  json = fileread ("tmp");
else
  json = urlread ("https://next.json-generator.com/api/json/get/4kpTYx-A7");
  save ("tmp", "json");
endif

addpath ("~/src/jsonlab/");

a = load_json(json);

#b = loadjson(json);
#assert (a, b)
    



