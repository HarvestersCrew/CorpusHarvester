all: download

run: download
	bin/download 4 http://wikimedia.com http://twitter.com

download: src/main.cpp
	mkdir -p bin
	g++ -o bin/download $^ -lcurl

.phony: clean

clean:
	rm -rf bin