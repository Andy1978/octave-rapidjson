#!/usr/bin/env octave

j = load_json(fileread (argv{1}));
puts(save_json (j));
