all: clean prepare compile run

# שלב שיוודא שהתיקיית bin קיימת
prepare:
	mkdir -p ./bin

compile:
	g++ -g -Wall -Weffc++ -std=c++11 -o ./bin/main src/* -Iinclude
	
run:
	./bin/main config_file.txt

clean:
	/usr/bin/rm -f ./bin/main
