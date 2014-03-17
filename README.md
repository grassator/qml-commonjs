Experiment in adding CommonJS support to QML.

## What works

* Full [modules](http://nodejs.org/api/modules.html) support
* `assert` module
* `setTimeout` / `clearTimeout` / `setInterval` / `clearInterval`
* experimental support for `util`, `events`, `path`, `url` and `querystring` modules.

When accessing top-level functions (like `setTimeout`) from QML, you have to use `CommonJS.` prefix because Qt doesn't allow defining global functions.

## Current goals

1. Access to `Buffer` object inside modules
2. Support for [fs](http://nodejs.org/api/fs.html) (right now only fs.existsSync() works)

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
