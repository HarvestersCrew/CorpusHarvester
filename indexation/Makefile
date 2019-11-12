all: 
	@ echo "Available tags are : sqlite, xapian, clear"

sqlite:
	mkdir -p bin/sqlite
	mkdir -p database
	g++ src/sqlite/create_database.cc -l sqlite3 -o bin/sqlite/create_database
	g++ src/sqlite/indexation.cc -l sqlite3 -o bin/sqlite/indexation
	g++ src/sqlite/search.cc -l sqlite3 -o bin/sqlite/search

xapian:
	mkdir -p bin/xapian
	g++ src/xapian/indexation.cc -l xapian -o bin/xapian/indexation

clear: 
	rm bin/**/*