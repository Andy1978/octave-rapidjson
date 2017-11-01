Some of the bigger JSON files are generated with

https://next.json-generator.com

It also has an API to directly read the JSON but I want to omit
something like `json = urlread ("https://next.json-generator.com/api/json/get/...");`
due to the posibility of a parser bug and thus possible remote code execution.
