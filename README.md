Experiment in adding CommonJS support to QML.

## What works

* Basic `require('./somefile.js')` statement for requiring files by relative path + `require.resolve` + `require.cache`
* `assert` module
* `setTimeout` / `clearTimeout` / `setInterval` / `clearInterval`
* experimental support for `util`, `events`, `path`, `url` and `querystring` modules.

When accessing top-level functions (like `setTimeout`) from QML, you have to use `CommonJS.` prefix because Qt doesn't allow defining global functions.

## Primary goals

1. Full support for [modules](http://nodejs.org/api/modules.html). *This is almost done with only recursive `node_modules` folder search remaining*.
2. At least partial support for [path](http://nodejs.org/api/path.html), [fs](http://nodejs.org/api/fs.html) and [util](http://nodejs.org/api/util.html).
3. Test popular real modules to see what is most required and implement based on demand.

## Not a concern

* Speed
* 100% feature parity with node.js

## Installation

* Make sure you have [Qt 5.2+](http://qt-project.org) installed.
* Clone this repository.
* Then run `qmake && make && make install`


## Usage

After that you should be able to just import CommonJS as regular QML library and use it in your code:

    import CommonJS 0.1
    import QtQuick 2.0
    
    Rectangle {
        Component.onCompleted: {
            CommonJS.setTimeout(function(){
                var example = CommonJS.require('./example.js');
            });
        }
    }
