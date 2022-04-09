#!/usr/bin/env bash
emcc --bind -s MODULARIZE=1 -s ALLOW_MEMORY_GROWTH=1 -s  EXPORT_ES6=1 -s ENVIRONMENT=web -s USE_BOOST_HEADERS=1 -o quick_example.js  cpp/src/*.cpp -I /usr/include/rapidjson
