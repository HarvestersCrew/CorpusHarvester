import 'package:example_flutter/data/source.dart';
import 'package:example_flutter/data/type.dart';
import 'package:flutter/material.dart';

class CorpusFile {
  final String name;
  final Source source;
  final FileType type;
  final DateTime date;

  CorpusFile({@required this.name, 
              @required this.source, 
              @required this.type, 
              @required this.date});
}