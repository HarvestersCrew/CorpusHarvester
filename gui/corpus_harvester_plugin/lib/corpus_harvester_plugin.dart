import 'dart:async';

import 'package:flutter/services.dart';

class CorpusHarvesterPlugin {
  static const MethodChannel _channel =
      const MethodChannel('corpus_harvester_plugin');

  static Future<String> get platformVersion async {
    final String version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }
}
