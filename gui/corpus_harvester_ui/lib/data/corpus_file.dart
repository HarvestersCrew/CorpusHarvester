import 'package:example_flutter/data/source.dart';
import 'package:example_flutter/data/type.dart';

class CorpusFile {
  String name;
  Source source;
  FileType type;
  DateTime date;

  CorpusFile(this.name, this.source, this.type, this.date);
}