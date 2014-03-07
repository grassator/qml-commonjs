console.log('Start Testing. Version: ' + process.version +
            ". Platform: " + process.platform);

// file doesn't exist
try {
    var main = require.resolve('./non-existent-file.js');
} catch(e) {
    console.log("Non-existent modules throw Errors as expected.");
}

// error inside file
try {
    var main = require('./main.js');
} catch(e) {
    console.log("Catching require error works");
}

var assert = require('assert');

try {
    ['path', 'querystring', 'events',
     'punycode', 'url', 'freelist'].forEach(function(module){
       assert.ok(require(module));
    });
    assert.equal("works", require('./no-package-json'));
    assert.equal("works", require('./package-json'));
} catch(e) {
    console.error(e);
}

console.log('All done.');
