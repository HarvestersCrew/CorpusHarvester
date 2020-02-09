import 'package:flutter/material.dart';
import 'package:intl/intl.dart';

import 'data/source.dart';
import 'data/type.dart';
import 'data/corpus_file.dart';
import 'data/corpus.dart';

class HistoryWidget extends StatefulWidget {
  HistoryWidget({Key key}) : super(key: key);

  @override
  _HistoryWidgetState createState() => _HistoryWidgetState();
}

Source twitterSource = Source(name: "Twitter");
Source wikiSource = Source(name: "WikiCommons");
FileType txtType = FileType(typeName: "Text");
FileType imgType = FileType(typeName: "Image");
FileType vidType = FileType(typeName: "Video");
CorpusFile fileA = CorpusFile(name: "A.txt", source: twitterSource, type: txtType, date: DateTime.now());
CorpusFile fileB = CorpusFile(name: "B.txt", source: wikiSource, type: txtType, date: DateTime.now());
CorpusFile fileC = CorpusFile(name: "C.txt", source: twitterSource, type: txtType, date: DateTime.now());
CorpusFile fileD = CorpusFile(name: "D.txt", source: twitterSource, type: txtType, date: DateTime.now());
CorpusFile fileE = CorpusFile(name: "E.txt", source: wikiSource, type: txtType, date: DateTime.now());
CorpusFile fileF = CorpusFile(name: "F.txt", source: wikiSource, type: txtType, date: DateTime.now());
CorpusFile fileLong = CorpusFile(name: "SuperlongfileLol.png", source: twitterSource, type: imgType, date: DateTime.now());
Corpus corpusA = Corpus(name: "Corpus A", date: DateTime.now().add(Duration(days: -2)), files: [fileA, fileC, fileE, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong, fileLong], sources: [twitterSource, wikiSource], types: [txtType]);
Corpus corpusB = Corpus(name: "Corpus Très très long !!!!", date: DateTime.now().add(Duration(days: -2)), files: [fileA, fileC, fileD], sources: [twitterSource], types: [txtType]);
Corpus corpusC = Corpus(name: "Corpus C", date: DateTime.now(), files: [fileB, fileE, fileF], sources: [wikiSource], types: [txtType]);
Corpus corpusD = Corpus(name: "Corpus D", date: DateTime.now(), files: [fileD, fileF], sources: [twitterSource, wikiSource], types: [txtType]);
List<Corpus> corpusList = [corpusA, corpusB, corpusC, corpusD];

class _HistoryWidgetState extends State<HistoryWidget> {

  Map<DateTime, List<Corpus>> _filteredMap;
  Corpus _selectedCorpus;

  @override
  void initState() {
    super.initState();
    _filteredMap = buildCorpusMap();
    _selectedCorpus = null;
    print(_filteredMap);
  }

  @override
  Widget build(BuildContext context) {
    return Row(
      children: <Widget>[
        Container(
          width: MediaQuery.of(context).size.width / 1.85,
          decoration: BoxDecoration(
            border: Border(
              right: BorderSide(
                color: Colors.black,
                width: 2.0
              )
            )
          ),
          child: Column(
            children: <Widget>[
              Container(
                height: 65,
                color: Colors.blue[400],
                child: Row(
                  children: <Widget>[
                    SizedBox(width: 20),
                    Icon(Icons.search, color: Colors.white),
                    SizedBox(width: 10),
                    Container(
                      width: MediaQuery.of(context).size.width / 7,
                      child: TextField(
                        style: TextStyle(color: Colors.white),
                        decoration: InputDecoration(
                          hintText: "Search for a corpus...",
                          hintStyle: TextStyle(color: Colors.blue[200]),
                          errorBorder: new UnderlineInputBorder(
                            borderSide: new BorderSide(
                              color: Colors.white,
                              width: 2.0
                            )
                          ),
                          focusedBorder: new UnderlineInputBorder(
                            borderSide: new BorderSide(
                              color: Colors.white,
                              width: 2.0
                            )
                          ),
                          focusedErrorBorder: new UnderlineInputBorder(
                            borderSide: new BorderSide(
                              color: Colors.white,
                              width: 2.0
                            )
                          ),
                          disabledBorder: new UnderlineInputBorder(
                            borderSide: new BorderSide(
                              color: Colors.white,
                              width: 2.0
                            )
                          ),
                          enabledBorder: new UnderlineInputBorder(
                            borderSide: new BorderSide(
                              color: Colors.white,
                              width: 2.0
                            )
                          )
                        ),
                      ),
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
                          initialDate: DateTime.now(),
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
                          });
                        });
                      },
                      child: Row(
                        children: <Widget>[
                          Text(""),
                          SizedBox(width: 10),
                          Icon(Icons.filter_list, color: Colors.white)
                        ]
                      ),           
                    )
                  ],
                ),
              ),
              Padding(
                padding: EdgeInsets.all(16.0),
                child: ListView.builder(
                  shrinkWrap: true,
                  itemCount: _filteredMap.length,
                  itemBuilder: (BuildContext context, int index) {
                    return Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        Row(
                          children: <Widget>[
                            Text(DateFormat('dd / MM / yyyy').format(_filteredMap.keys.elementAt(index)), style: TextStyle(fontSize: 16, color: Colors.grey)),
                            SizedBox(width: 16),
                            Expanded(child: Divider(thickness: 2.0))
                          ]
                        ),
                        SizedBox(height: 10.0),
                        buildCorpusCards(_filteredMap.values.elementAt(index)),
                        SizedBox(height: 25.0)
                      ],
                    );
                  }
                )
              )
            ],
          )
        ),
        Expanded(
          child: _selectedCorpus != null ? ListView(
            //crossAxisAlignment: CrossAxisAlignment.center,
            children: <Widget>[
              Padding(
                padding: EdgeInsets.all(40.0),
                child: Text(_selectedCorpus.name, textAlign: TextAlign.center, style: TextStyle(fontSize: 40, fontWeight: FontWeight.bold)),
              ),
              Divider(thickness: 2.0),
              SizedBox(height: 30.0),
              Text("Criteria", textAlign: TextAlign.center, style: TextStyle(fontSize: 25.0, fontWeight: FontWeight.bold)),
              Padding(
                padding: EdgeInsets.only(left: 40.0, right: 40.0, top: 30.0, bottom: 30.0),
                child: Row(
                  mainAxisSize: MainAxisSize.max,
                  children: <Widget>[
                    Column(
                      crossAxisAlignment: CrossAxisAlignment.start,
                      children: <Widget>[
                        Text("Type(s) : " + _selectedCorpus.buildTypesString(), style: TextStyle(fontSize: 20.0)),
                        Text("Date : " + DateFormat('dd / MM / yyyy').format(_selectedCorpus.date).toString(), style: TextStyle(fontSize: 20.0)),
                        Text("Source(s) : " + _selectedCorpus.buildSourcesString(), style: TextStyle(fontSize: 20.0)),
                      ],
                    )
                  ],
                ),
              ),
              Divider(thickness: 2.0),
              SizedBox(height: 30.0),
              Text("Corpus files", textAlign: TextAlign.center, style: TextStyle(fontSize: 25.0, fontWeight: FontWeight.bold)),
              buildFilesIcons(_selectedCorpus.files)
            ],
          ) : Container()
        )
      ],
    );
  }

  Map<DateTime, List<Corpus>> buildCorpusMap() {
    List<Corpus> _corpuses = corpusList;
    _corpuses.sort((a,b) => b.date.compareTo(a.date));
    List<DateTime> dates = [];
    Map<DateTime, List<Corpus>> result = {};
    for(Corpus c in _corpuses) {
      if(!dates.contains(new DateTime(c.date.year, c.date.month, c.date.day))) dates.add(new DateTime(c.date.year, c.date.month, c.date.day));
    }
    for(DateTime dt in dates) {
      result.putIfAbsent(dt, () => _corpuses.where((corpus) => corpus.date.difference(dt).inDays == 0).toList());
    }
    return result;
  }

  Widget buildCorpusCards(List<Corpus> corpuses) {
    List<Widget> cards = [];
    for(Corpus corpus in corpuses) {
      cards.add(
        new GestureDetector(
          child: Card(
            color: corpus == _selectedCorpus ? Colors.blue[300] : Colors.white,
            elevation: 3.0,
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(12.0),
            ),
            child: Padding(
              padding: EdgeInsets.all(16.0),
              child: ConstrainedBox(
                constraints: BoxConstraints(
                  minWidth: MediaQuery.of(context).size.width / 12,
                  maxWidth: MediaQuery.of(context).size.width / 12
                ),
                child: Text(corpus.name, overflow: TextOverflow.ellipsis, textAlign: TextAlign.center, style: TextStyle(fontSize: 20, color: corpus == _selectedCorpus ? Colors.white : Colors.black)),
              ),
            )
          ),
          onTap: () {
            setState(() {
              _selectedCorpus = _selectedCorpus == corpus ? null : corpus;
            });
          },
        )
      );
    }
    return new Wrap(
      children: cards
    );
  }

  Widget buildFilesIcons(List<CorpusFile> files) {
    List<Widget> icons = [];
    for(CorpusFile file in files) {
      icons.add(
        new Padding(
          padding: EdgeInsets.all(30.0),
          child: Column(
            children: <Widget>[
              file.type.typeName == "Text" ? 
                Icon(Icons.text_fields, size: 70.0) : 
                file.type.typeName == "Image" ?
                Icon(Icons.image, size: 70.0) : 
                file.type.typeName == "Video" ?
                  Icon(Icons.movie, size: 70.0) :
                  Icon(Icons.insert_drive_file, size: 70.0),
              ConstrainedBox(
                  constraints: BoxConstraints(
                    minWidth: 100.0,
                    maxWidth: 100.0
                  ),
                  child: Text(file.name, overflow: TextOverflow.ellipsis, textAlign: TextAlign.center, style: TextStyle(fontSize: 20)),
                )
            ],
          ),
        )
      );
    }
    return new Wrap(
      children: icons
    );
  }

  
}