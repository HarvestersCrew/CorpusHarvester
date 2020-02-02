import 'dart:async';

import 'package:example_flutter/data/corpus_file.dart';
import 'package:example_flutter/data/source.dart';
import 'package:example_flutter/data/type.dart';
import 'package:flutter/material.dart';
import 'package:intl/intl.dart';

class MyHomePage extends StatefulWidget {
  MyHomePage({Key key}) : super(key: key);

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

Source twitterSource = Source("Twitter");
Source wikiSource = Source("WikiCommons");
FileType txtType = FileType("Text");
FileType imgType = FileType("Image");
FileType vidType = FileType("Video");
CorpusFile fileA = CorpusFile("A.txt", twitterSource, txtType, DateTime.now());
CorpusFile fileB = CorpusFile("B.txt", wikiSource, txtType, DateTime.now());
CorpusFile fileC = CorpusFile("C.txt", twitterSource, txtType, DateTime.now());
CorpusFile fileD = CorpusFile("D.txt", twitterSource, txtType, DateTime.now());
CorpusFile fileE = CorpusFile("E.txt", wikiSource, txtType, DateTime.now());
CorpusFile fileF = CorpusFile("F.txt", wikiSource, txtType, DateTime.now());

List<Source> _sources = [twitterSource, wikiSource];
List<FileType> _types = [txtType, imgType, vidType];
List<CorpusFile> _filesList = [fileA, fileB, fileC, fileD, fileE, fileF];

class _MyHomePageState extends State<MyHomePage> {

  List<Source> _sourcesFilters;
  List<bool> _sourcesCheckboxes;
  List<FileType> _typesFilters;
  List<bool> _typesCheckboxes;
  List<CorpusFile> _filteredList;
  DateTime _selectedDate;

  @override
  void initState() {
    super.initState();
    _sourcesFilters = List.of(_sources);
    _sourcesCheckboxes = [true, true];
    _typesFilters = List.of(_types);
    _typesCheckboxes = [true, true, true];
    _filteredList = List.of(_filesList);
    _selectedDate = null;
  }

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      length: 2,
      child: Scaffold(
        appBar: AppBar(
          title: Center(child: Text("Corpus Harvester UI"),),
          bottom: TabBar(
            tabs: <Widget>[
              Tab(text: "Création", icon: Icon(Icons.create)),
              Tab(text: "Historique", icon: Icon(Icons.history)),
            ],
          ),
        ),
        body: TabBarView(
          children: <Widget>[
            showCreationWidget(),
            showHistoryWidget(),
          ],
        ),
        floatingActionButton: FloatingActionButton(
          onPressed: () {},
          child: Icon(Icons.add),
          tooltip: "Create corpus with selected files",
          hoverColor: Colors.blue[300],
        ),
      ),
    );
  }

  Widget showCreationWidget() {
    return Column(
      children: <Widget>[
        Container(
          height: 65,
          color: Colors.blue[400],
          child: Row(
            children: <Widget>[
              SizedBox(width: 20),
              Text("Type ", style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold)),
              SizedBox(width: 10),
              GestureDetector(
                child: Icon(Icons.filter_list, color: Colors.white),
                onTap: () {
                  showDialog(
                    context: context,
                    builder: (BuildContext context) {
                      return StatefulBuilder(
                        builder: (context, setState) {
                          return AlertDialog(
                            title: Text("Choose Type filters"),
                            content: Container(
                              height: MediaQuery.of(context).size.height / 3.0,
                              width: MediaQuery.of(context).size.width / 5.0,
                              child: Column(
                                children: <Widget>[
                                  ListView.builder(
                                    shrinkWrap: true,
                                    itemCount: _types.length,
                                    itemBuilder: (BuildContext context, int index) {
                                      return CheckboxListTile(
                                        title: Text(_types.elementAt(index).typeName),
                                        value: _typesCheckboxes.elementAt(index),
                                        onChanged: (bool value) {
                                          setState(() {
                                            _typesCheckboxes.replaceRange(index, index + 1, [value]);
                                            if(value) _typesFilters.add(_types.elementAt(index));
                                            else _typesFilters.remove(_types.elementAt(index));
                                          });
                                        },
                                      );
                                    },
                                  ),
                                  FlatButton(
                                    child: Text("Select all", style: TextStyle(fontSize: 18)),
                                    textColor: Colors.blue,
                                    onPressed: () {},
                                  ),
                                ],
                              ),
                            ),
                            actions: <Widget>[
                              FlatButton(
                                child: Text('Ok'),
                                onPressed: () async {
                                  Navigator.of(context).pop();
                                  await rebuildFilteredList();
                                },
                              ),
                            ],
                          );
                        },
                      );
                    }
                  );
                },
              ),
              SizedBox(width: 10),
              Padding(
                padding: EdgeInsets.only(top: 15, bottom: 15),
                child: VerticalDivider(color: Colors.white, thickness: 2.0),
              ),
              SizedBox(width: 10),
              Text("Source ", style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold)),
              SizedBox(width: 10),
              GestureDetector(
                child: Icon(Icons.filter_list, color: Colors.white),
                onTap: () {
                  showDialog(
                    context: context,
                    builder: (BuildContext context) {
                      return StatefulBuilder(
                        builder: (context, setState) {
                          return AlertDialog(
                            title: Text("Choose Source filters"),
                            content: Container(
                              height: MediaQuery.of(context).size.height / 3.0,
                              width: MediaQuery.of(context).size.width / 5.0,
                              child: ListView.builder(
                                shrinkWrap: true,
                                itemCount: _sources.length,
                                itemBuilder: (BuildContext context, int index) {
                                  return CheckboxListTile(
                                    title: Text(_sources.elementAt(index).name),
                                    value: _sourcesCheckboxes.elementAt(index),
                                    onChanged: (bool value) {
                                      setState(() {
                                        _sourcesCheckboxes.replaceRange(index, index + 1, [value]);
                                        if(value) _sourcesFilters.add(_sources.elementAt(index));
                                        else _sourcesFilters.remove(_sources.elementAt(index));
                                      });
                                    },
                                  );
                                },
                              ),
                            ),
                            actions: <Widget>[
                              FlatButton(
                                child: Text('Ok'),
                                onPressed: () async {
                                  Navigator.of(context).pop();
                                  await rebuildFilteredList();
                                },
                              ),
                            ],
                          );
                        },
                      );
                    }
                  );
                },
              ),
              SizedBox(width: 10),
              Padding(
                padding: EdgeInsets.only(top: 15, bottom: 15),
                child: VerticalDivider(color: Colors.white, thickness: 2.0),
              ),
              SizedBox(width: 10),
              //Text("Date ", style: TextStyle(color: Colors.white, fontSize: 20, fontWeight: FontWeight.bold)),
              Icon(Icons.calendar_today, color: Colors.white),
              SizedBox(width: 10),
              GestureDetector(
                onTap: () async {
                  showDatePicker(
                    context: context,
                    initialDate: _selectedDate != null ? _selectedDate : DateTime.now(),
                    firstDate: DateTime(1970),
                    lastDate: DateTime.now(),
                    builder: (BuildContext context, Widget child) {
                      return Center(
                        child: SizedBox(
                          width: MediaQuery.of(context).size.width / 2.5,
                          height: MediaQuery.of(context).size.height / 2.5,
                          child: child,
                        )
                      );
                    },
                  ).then((value) {
                    setState(() {
                      _selectedDate = value;
                    });
                    rebuildFilteredList();
                  });
                },
                child: Row(
                  children: <Widget>[
                    Text(_selectedDate != null ? DateFormat('dd / MM / yyyy').format(_selectedDate) : "           ", style: TextStyle(color: Colors.white, fontSize: 20)),
                    SizedBox(width: 10),
                    Icon(Icons.arrow_drop_down, color: Colors.white)
                  ],
                ),                
              )
            ],
          ),
        ),
        Container(
          height: 50,
          color: Colors.grey[350],
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Container(
                width: (MediaQuery.of(context).size.width - 150) / 4.0,
                child: Text("File name", style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold), textAlign: TextAlign.center),
              ),
              Container(
                width: (MediaQuery.of(context).size.width - 150) / 4.0,
                alignment: Alignment.center,
                child: Text("Source", style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
              ),
              Container(
                width: (MediaQuery.of(context).size.width - 150) / 4.0,
                alignment: Alignment.center,
                child: Text("Type", style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
              ),
              Container(
                width: (MediaQuery.of(context).size.width - 150) / 4.0,
                alignment: Alignment.center,
                child: Text("Date", style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
              ),
            ],
          ),
        ),
        Expanded(
          child: ListView.builder(
            shrinkWrap: true,
            itemCount: _filteredList.length,
            itemBuilder: (BuildContext context, int index) {
              return new Padding(
                padding: EdgeInsets.all(5.0),
                child: Row(
                mainAxisSize: MainAxisSize.max,
                children: <Widget>[
                  Container(
                    width: 50,
                      child: Checkbox(
                      value: false, 
                      onChanged: (value) {}
                    ),
                  ),
                  Expanded(
                    child: Row(
                      mainAxisAlignment: MainAxisAlignment.center,
                      children: <Widget>[
                        Container(
                          width: (MediaQuery.of(context).size.width - 150) / 4.0,
                          alignment: Alignment.center,
                          child: Text(_filteredList.elementAt(index).name, style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold), textAlign: TextAlign.center),
                        ),
                        Container(
                          width: (MediaQuery.of(context).size.width - 150) / 4.0,
                          alignment: Alignment.center,
                          child: Text(_filteredList.elementAt(index).source.name, style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
                        ),
                        Container(
                          width: (MediaQuery.of(context).size.width - 150) / 4.0,
                          alignment: Alignment.center,
                          child: Text(_filteredList.elementAt(index).type.typeName, style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
                        ),
                        Container(
                          width: (MediaQuery.of(context).size.width - 150) / 4.0,
                          alignment: Alignment.center,
                          child: Text(DateFormat('dd / MM / yyyy').format(_filteredList.elementAt(index).date), style: TextStyle(fontSize: 18), textAlign: TextAlign.center),
                        ),
                      ],
                    ),
                  ),
                  Container(
                    width: 50,
                    child: Icon(Icons.more_vert),
                  ),
                ],
              ),
              );
            },
          ),
        )
      ],
    );
  }

  Widget showHistoryWidget() {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: <Widget>[
        Icon(Icons.history),
        SizedBox(height: 10),
        Text("Historique"),
      ],
    );
  }

  Future<void> rebuildFilteredList() async {
    _filteredList.clear();
    for(CorpusFile cf in _filesList) {
      if(_sourcesFilters.contains(cf.source) && _typesFilters.contains(cf.type) && checkDate(cf.date)) _filteredList.add(cf);
    }
    setState(() {});
  }

  bool checkDate(DateTime date) {
    return _selectedDate == null || _selectedDate.difference(date).inDays == 0;
  }
}