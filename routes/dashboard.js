var express = require('express');
var router = express.Router();

router.get('/', function(req, res, next) {
  var json = {};
  json.partials =
              {
                head: 'head',
                body: 'chart',
                header: 'header',
                footer: 'footer'
              };
  res.charset = 'utf-8';
  res.render('dashboard', json);
});

module.exports = router;
