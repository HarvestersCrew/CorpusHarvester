/** @file simplesearch.cc
 * @brief Simple command-line search utility.
 *
 * See "quest" for a more sophisticated example.
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

#include <xapian.h>

#include <iostream>
#include <string>

#include <cstdlib> // For exit().
#include <cstring>

using namespace std;

enum FileMetadata {
  FILE_PATH,
  FILE_TYPE,
  FILE_SIZE,
};

void print_usage(string prog);
void print_usage(string prog) {
  cout << "Usage: " << prog << " <PATH_TO_DATABASE> <QUERY_TYPE> <QUERY>\n"
       << endl;
  cout << "<QUERY_TYPE> :\n"
       << "\t-c : file content query\n"
       << "\t-p : file path query\n"
       << "\t-t : file type query\n"
       << endl;
}

int main(int argc, char **argv) try {
  // We require at least two command line arguments.
  if (argc < 3) {
    int rc = 1;
    if (argv[1]) {
      if (strcmp(argv[1], "--version") == 0) {
        cout << "simplesearch" << endl;
        exit(0);
      }
      if (strcmp(argv[1], "--help") == 0) {
        rc = 0;
      }
    }
    print_usage(argv[0]);
    exit(rc);
  } else if (argc == 3 || argv[2][0] != '-') {
    print_usage(argv[0]);
    exit(2);
  }

  // Open the database for searching.
  Xapian::Database db(argv[1]);

  // Start an enquire session.
  Xapian::Enquire enquire(db);

  Xapian::Query query;
  if (strcmp(argv[2], "-c") == 0) {
    // Combine the rest of the command line arguments with spaces between
    // them, so that simple queries don't have to be quoted at the shell
    // level.
    string query_string(argv[2]);
    argv += 3;
    while (*argv) {
      query_string += ' ';
      query_string += *argv++;
    }

    // Parse the query string to produce a Xapian::Query object.
    Xapian::QueryParser qp;
    Xapian::Stem stemmer("english");
    qp.set_stemmer(stemmer);
    qp.set_database(db);
    qp.set_stemming_strategy(Xapian::QueryParser::STEM_SOME);
    query = qp.parse_query(query_string);
  } else if (strcmp(argv[2], "-p") == 0) {
    query = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, FILE_PATH, argv[3],
                          argv[3]);
  } else if (strcmp(argv[2], "-t") == 0) {
    query = Xapian::Query(Xapian::Query::OP_VALUE_RANGE, FILE_TYPE, argv[3],
                          argv[3]);
  } else {
    print_usage(argv[0]);
    exit(3);
  }

  cout << "Parsed query is: " << query.get_description() << endl;

  // Find the top 10 results for the query.
  enquire.set_query(query);
  Xapian::MSet matches = enquire.get_mset(0, 10);

  // Display the results.
  cout << matches.get_matches_estimated() << " results found.\n";
  cout << "Matches 1-" << matches.size() << ":\n" << endl;

  for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i) {
    cout << i.get_rank() + 1 << ": " << i.get_weight() << " docid=" << *i
         << " [" << i.get_document().get_data() << "]\n\n";
  }
} catch (const Xapian::Error &e) {
  cout << e.get_description() << endl;
  exit(1);
}
