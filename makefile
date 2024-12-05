all: clean prepare compile run

prepare:
	mkdir -p ./bin

compile:
	g++ -g -Wall -Weffc++ -std=c++11 -o ./bin/simulation src/* -Iinclude

run:
	./bin/simulation config_file.txt

clean:
	/usr/bin/rm -f ./bin/simulation
