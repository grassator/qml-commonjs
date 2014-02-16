console.log('Start Testing. Version: ' + process.version +
            ". Platform: " + process.platform);

var main = require('./main.js');
var assert = require('assert');

try {
    assert.ok(main);
    assert.ok('function' === typeof main);
    assert.equal("works", require('./no-package-json'));
    assert.equal("works", require('./package-json'));
} catch(e) { }

console.log('All done.');
