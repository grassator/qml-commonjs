console.log('Start Testing. Version: ' + process.version +
            ". Platform: " + process.platform);

try {
    var main = require('./main.js');
} catch(e) {
    console.error(e);
}

var assert = require('assert');

try {
    ['path', 'querystring', 'events'].forEach(function(module){
       assert.ok(require(module));
    });
    assert.equal("works", require('./no-package-json'));
    assert.equal("works", require('./package-json'));
} catch(e) { }

console.log('All done.');
