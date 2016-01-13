var cfg = require('../etc/app_config.json');

var getProp = function (type, subtype) {
  if (!cfg[type]) {
    return null;
  }

  return cfg[type];
}
exports.getProp = getProp;

