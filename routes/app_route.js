var express = require('express');
var router = express.Router({
  caseSensitive:        true,
  strict:               true
});

var fs = require('fs');
var path = require('path');
var net = require('net');
var url = require('url');

var utl = require('../modules/util.js');
var cfg = require('../modules/config.js');
var log = require('../modules/logger.js');

router.use('/dashboard', require('./dashboard.js'));
router.use('/data', require('./data-restapi.js'));

router.get('/', function(req, res) {
  res.redirect('dashboard/');
});

router.get('/head', function(req, res) {
  if (!req.xhr) {
    res.sendStatus(404);
    return;
  }
  res.charset = 'utf-8';
  res.render('head_metro');
});

router.get('/dashboard', function(req, res) {
  res.redirect('dashboard/');
});

router.get('/data', function(req, res) {
  res.redirect('data/');
});

module.exports = router;

