Experiment in adding CommonJS support to QML.

## What works

Right now only basic `require()` statement for requiring files (not modules). No cyclic dependency checks.

## Goals

1. Full support for [globals](http://nodejs.org/api/globals.html) and [modules](http://nodejs.org/api/modules.html).
2. At least partial support for [path](http://nodejs.org/api/path.html), [fs](http://nodejs.org/api/fs.html) and [util](http://nodejs.org/api/util.html)
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

    import CommonJS 1.0
    import QtQuick 2.0
    
    Rectangle {
        Component.onCompleted: {
            var example = CommonJS.require('./example.js');
        }
    }

Inside QML it has to be called as `CommonJS.require` because it is impossible to define top-level function. Inside required code `require` is available as global function.