/** @file simpleindex.cc
 * @brief Index each paragraph of a text file as a Xapian document.
 */
/* Copyright (C) 2007,2010,2015 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib> // For exit().
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <xapian.h>

using namespace std;

enum FileMetadata {
  FILE_PATH,
  FILE_TYPE,
  FILE_SIZE,
};

int already_indexed(string file_path, Xapian::WritableDatabase db);
void index_tweet(string file_path, Xapian::TermGenerator indexer,
                 Xapian::WritableDatabase db);
void print_usage(string prog, int code);
void clean_database(Xapian::WritableDatabase db);

int already_indexed(string file_path, Xapian::WritableDatabase db) {
  for (Xapian::ValueIterator i = db.valuestream_begin(FILE_PATH);
       i != db.valuestream_end(FILE_PATH); ++i) {
    if (file_path == *i) {
      return 1;
    }
  }
  return 0;
}

void index_tweet(string file_path, Xapian::TermGenerator indexer,
                 Xapian::WritableDatabase db) {
  std::ifstream tweet(file_path);
  string tweet_text;
  string line;
  Xapian::docid did = 0;
  while (!tweet.eof()) {
    getline(tweet, line);
    tweet_text += line;
  }

  // We've reached the end of a tweet_textgraph, so index it.
  Xapian::Document doc;
  doc.set_data(tweet_text);
  doc.add_value(FILE_PATH, file_path);
  doc.add_value(FILE_TYPE, "tweet");
  //...
  indexer.set_document(doc);
  indexer.index_text(tweet_text);

  // Add the document to the database.
  did = db.add_document(doc);
  cout << "File " << file_path
       << " is a tweet and represents the xapian document n°" << did << endl;
}

void print_usage(string prog, int code) {
  cout << "Error " << code << "\nUsage: " << prog
       << " PATH_TO_DATABASE\n"
          "Index each paragraph of a text file as a Xapian document."
       << endl;
  exit(code);
}

void clean_database(Xapian::WritableDatabase db) {
  int deleted_doc_count = 0;
  for (Xapian::PostingIterator i = db.postlist_begin("");
       i != db.postlist_end(""); ++i) {
    db.delete_document(*i);
    deleted_doc_count++;
  }
  db.commit();
  if (deleted_doc_count > 0) {
    cout << "Database was correctly cleaned : " << deleted_doc_count
         << " documents deleted" << endl;
  }
}

/* MAIN */
int main(int argc, char **argv) try {
  if (argc == 2) {
    int rc = 1;
    if (argv[1][0] == '-') {
      if (strcmp(argv[1], "--version") == 0) {
        cout << "simpleindex" << endl;
        exit(0);
      }
      if (strcmp(argv[1], "--help") == 0) {
        rc = 0;
      }
      print_usage(argv[0], rc);
      exit(0);
    }
  }

  // If 3 arguments then check if at least one is an option
  if (argc == 3 && !(argv[1][0] == '-' || argv[2][0] == '-')) {
    cout << argv[1] << endl;
    print_usage(argv[0], 2);
  }

  const char *option = argv[1];
  const char *database = argv[1];
  if (argc == 3 && database[0] == '-') {
    option = argv[1];
    database = argv[2];
  }
  // Open the database for update, creating a new database if necessary.
  Xapian::WritableDatabase db(database, Xapian::DB_CREATE_OR_OPEN);

  if (strcmp(option, "--clean-database") == 0 || strcmp(option, "-cdb") == 0) {
    clean_database(db);
    cout << "Document count : " << db.get_doccount() << endl;
    exit(0);
  }

  Xapian::TermGenerator indexer;
  Xapian::Stem stemmer("english");
  indexer.set_stemmer(stemmer);
  indexer.set_stemming_strategy(indexer.STEM_SOME_FULL_POS);

  DIR *tweets;
  struct dirent *entry;
  string file_path = "";
  int added_doc_count = 0;
  if ((tweets = opendir("./tweets"))) {
    while ((entry = readdir(tweets))) {
      // cout << entry->d_name << endl;
      if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        file_path = entry->d_name;
        file_path = "./tweets/" + file_path;
        if (!already_indexed(file_path, db)) {
          index_tweet(file_path, indexer, db);
          added_doc_count++;
        } else {
          cout << file_path << " is already indexed" << endl;
        }
      }
    }
    closedir(tweets);
  }

  // Explicitly commit so that we get to see any errors.  WritableDatabase's
  // destructor will commit implicitly (unless we're in a transaction) but
  // will swallow any exceptions produced.
  db.commit();
  if (added_doc_count > 0) {
    cout << "Database was correctly filled with the " << added_doc_count
         << " previous documents" << endl;
  }
  cout << "Number of documents in the database : " << db.get_doccount() << endl;

  /* db.delete_document(did);
  cout << "Document " << did << " removed properly from database" << endl; */
} catch (const Xapian::Error &e) {
  cout << e.get_description() << endl;
  exit(1);
}
