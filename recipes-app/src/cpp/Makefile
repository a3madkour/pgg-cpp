##
# Project Title
#
# @file
# @version 0.1


all: test-case

rerun: clean run

run: test-case
	build/test-case
clean:
	rm -rf build/test-case
test-case:
	g++ -o build/test-case src/*.cpp -fpic -Wall -lboost_serialization -lboost_system -I /usr/include/rapidjson

# end
