var express = require('express');
var router = express.Router();

var fs = require('fs');
var path = require('path');

var log = require('../modules/logger.js');
var rdb = require('../modules/rdb.js');


var responseDHTJSON = function (req, res) {
  rdb.queryTimeSeriesDHT(function (err, datajson) {
    log.info('GET JSON from ' + req.ip + ' ' + req.ips);
    res.charset = 'utf-8';
    res.type('application/json');
    //res.json(datajson);
    //res.send(JSON.stringify(datajson, null, '  '));
    res.send(datajson);
  });
}
router.get('/', responseDHTJSON);

module.exports = router;

