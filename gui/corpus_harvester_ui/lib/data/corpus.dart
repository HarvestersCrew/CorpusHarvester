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

  String buildSourcesString() {
    String result = "";
    if(sources.length == 0) result = "undefined";
    else {
      result += sources[0].name;
      for(int i = 1; i < sources.length; i++) {
        result += ", ${sources[i].name}";
      }
    }
    return result;
  }

  String buildTypesString() {
    String result = "";
    if(types.length == 0) result = "undefined";
    else {
      result += types[0].typeName;
      for(int i = 1; i < types.length; i++) {
        result += ", ${types[i].typeName}";
      }
    }
    return result;
  }
}