import 'package:example_flutter/data/corpus_file.dart';
import 'package:example_flutter/data/source.dart';
import 'package:example_flutter/data/type.dart';
import 'package:flutter/material.dart';

class Corpus {
  final String name;
  final List<CorpusFile> files;
  final List<Source> sources;
  final List<FileType> types;
  final DateTime date;

  Corpus({@required this.name,
          @required this.files, 
          @required this.sources, 
          @required this.types, 
          @required this.date});
}