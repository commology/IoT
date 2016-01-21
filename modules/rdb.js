var fs = require('fs');
var path = require('path');
var url = require('url');

var utl = require('./util.js');
var log = require('./logger.js');
var cfg = require('./config.js');

var redis = require('redis');
var redis_cli = redis.createClient();

redis_cli.on('error', function (err) {
  log.error('Redis: ' + err);
  throw Error(err); 
});

redis_cli.select(2, function () {
});


queryTimeSeries = function (keys_pattern, callback, begin_timestamp, end_timestamp) {
  redis_cli.keys(keys_pattern, function (err, keys) {
    okeys = keys.sort();
    redis_cli.mget(okeys, function (err, vals) {
      callback(null, {'keys': okeys, 'vals': vals});
    });
  });
}
exports.queryTimeSeries = queryTimeSeries;

exports.queryTimeSeriesDHT = function (callback, begin_timestamp, end_timestamp) {
  var timestamp = [];
  var timestring = [];
  var humidity = [];
  var temperature = [];
  queryTimeSeries('*:*', function (err, result) {
    var keys = result.keys;
    var vals = result.vals;
    keys.forEach(function (element, index, array) {
      var k = parseInt(element.split(':')[0]);
      var d = new Date(0);
      d.setUTCSeconds(k);
      //var s = utl.stringifyDate(d);
      var s = d.toISOString(d);
      var v = parseFloat(vals[index]);
      if (index % 2 == 0) {
        timestamp.push(k);
        timestring.push(s);
        humidity.push(v);
      }
      else {
        temperature.push(v);
      }
    });
    var datajson = {
      'timestamp': timestamp,
      'timestring' : timestring,
      'humidity': humidity,
      'temperature': temperature
    };
    callback(null, datajson);
  });
}

/*
  redis_cli.hgetall(redis_key, function (err, hitems) {
    redis_cli.hkeys(redis_key, function (err, hkey) {
      var hkeys = [];
      var hvals = [];
      var data = null;
      hkey.forEach(function (element, index, array) {
        var timestamp = parseInt(element);
        if (end_timestamp) {
          if (timestamp > end_timestamp)
            // ignore 
            return true;
        }
        else
          end_timestamp = 0;
  
        if (begin_timestamp) {
          if (timestamp < begin_timestamp) {
            // ignore 
            return true;
          }
        }
        else
          begin_timestamp = 0;
        
        hkeys.push(timestamp);
        if (hitems.hasOwnProperty(element)) {
          data = parseInt(hitems[element]);
          if (isNaN(data)) {
            data = hitems[element];
            if (data == 'true')
              data = true;
            if (data == 'false')
              data = false;
          }
          hvals.push(data);
          dataset.push(
          {
            'key': timestamp,
            'val': data
          });
        }
        else {
          hvals.push(null);
          dataset.push(
          {
            'key': timestamp,
            'val': null
          });
        }
      });
      //callback(hkeys, hvals);
      callback(dataset);
    });
  });
*/

