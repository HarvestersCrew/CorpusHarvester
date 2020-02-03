import 'package:flutter/material.dart';

class HomePage extends StatefulWidget {
  const HomePage({
      Key key, 
      this.focusedIndex, 
      @required this.leadings, 
      @required this.views
    }) : assert(leadings != null),
         assert(views != null),
         super(key: key);

  final int focusedIndex;
  final List<IconData> leadings;
  final List<Widget> views;

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  int _focusedIndex;
  List<IconData> _leadings;
  List<Widget> _views;

  @override
  void initState() {
    super.initState();
    _views = widget.views;
    _leadings = widget.leadings;
    _focusedIndex = widget.focusedIndex == null ? 0 : widget.focusedIndex;
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Row(
        children: <Widget>[
          Container(
            width: MediaQuery.of(context).size.width / 30,
            color: Colors.blue,
            child: ListView.builder(
              itemCount: _leadings.length,
              itemBuilder: (BuildContext context, int index) {
                return GestureDetector(
                  child: Container(
                    width: MediaQuery.of(context).size.width / 30,
                    height: MediaQuery.of(context).size.width / 30,
                    child: Icon(_leadings.elementAt(index), size: 30, color: Colors.white),
                    decoration: BoxDecoration(
                      border: Border(
                        left: BorderSide(
                          color: index == _focusedIndex ? Colors.white : Colors.blue,
                          width: 5.0
                        )
                      )
                    ),
                  ),
                  onTap: () {
                    setState(() {
                      _focusedIndex = index;
                    });
                  },
                );
              }
            )
          ),
          Expanded(
            child: _views.elementAt(_focusedIndex)
          )
        ],
      ),
    );
  }

}