// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

import 'package:example_flutter/creation_widget.dart';
import 'package:example_flutter/history_widget.dart';
import 'package:example_flutter/home_page.dart';
import 'package:flutter/foundation.dart'
    show debugDefaultTargetPlatformOverride;
import 'package:flutter/material.dart';
import 'package:corpus_harvester_plugin/corpus_harvester_plugin.dart' as corpus_harvester_plugin;

void main() {
  // See https://github.com/flutter/flutter/wiki/Desktop-shells#target-platform-override
  debugDefaultTargetPlatformOverride = TargetPlatform.fuchsia;

  runApp(new MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    print("HERE !");
    corpus_harvester_plugin.CorpusHarvesterPlugin.platformVersion.then((version) {
      print(version);
    });
    return MaterialApp(
      title: 'Corpus Harvester UI',
      debugShowCheckedModeBanner: false,
      theme: ThemeData(
        primarySwatch: Colors.blue,
        // See https://github.com/flutter/flutter/wiki/Desktop-shells#fonts
        fontFamily: 'Roboto',
      ),
      home: HomePage(
        leadings: <IconData>[Icons.create, Icons.history],
        views: <Widget>[CreationWidget(), HistoryWidget()],
      ),
    );
  }
}


