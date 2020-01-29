import 'package:example_flutter/data/corpus_file.dart';
import 'package:example_flutter/data/source.dart';
import 'package:example_flutter/data/type.dart';

class Corpus {
  List<CorpusFile> files;
  List<Source> sources;
  List<FileType> types;
  DateTime date;

  Corpus({this.files, sources, types, date});
}