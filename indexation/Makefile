all: 
	@ echo "Available tags are : sqlite, xapian, clear"

sqlite:
	g++ src/sqlite/create_database.cc -l sqlite3 -o bin/sqlite/create_database
	g++ src/sqlite/indexation.cc -l sqlite3 -o bin/sqlite/indexation

xapian:
	g++ src/xapian/indexation.cc -l xapian -o bin/xapian/indexation

clear: 
	rm bin/**/*