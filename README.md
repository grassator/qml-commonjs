# DISCONTINUED

> **Notice:** Unfortunately I can not commit to supporting and further development of this project. Please check [Quickly](https://github.com/quickly/quickly/) which provides similar, or better functionality. If you are still interested in becoming an owner / maintainer please submit an issue and I will get back to you.

# QML CommonJS

Experiment in adding CommonJS module support to QML.

## Changelog

### Version 0.2

* Fixed compatibility with Qt 5.4+ (see changes below for details).
* **CHANGED** paths for `CommonJS.require` calls from QML are now resolved relative to `QmlEngine::baseUrl()` because evaluating `Qt.resolvedUrl('.')` no longer works from CPP code for some reason.
* **CHANGED** evaluated module is now wrapped in an [IIFE](https://en.wikipedia.org/wiki/Immediately-invoked_function_expression) to avoid weird problem where function with a name same a property on a global object (e.g. `hasOwnProperty`) would result in an error.

### Version 0.1

* Full [modules](http://nodejs.org/api/modules.html) support
* `assert` module
* `setTimeout` / `clearTimeout` / `setInterval` / `clearInterval`
* experimental support for `util`, `events`, `path`, `url` and `querystring` modules.

When accessing top-level functions (like `setTimeout`) from QML, you have to use `CommonJS.` prefix because Qt doesn't allow defining global functions.

## Installation

* Make sure you have [Qt 5.4+](https://www.qt.io/) installed.
* Clone this repository.
* Then run `qmake && make && make install`

## Usage

After that you should be able to just import CommonJS as regular QML library and use it in your code:

```js
import CommonJS 0.2
import QtQuick 2.0

Rectangle {
    Component.onCompleted: {
        CommonJS.setTimeout(function(){
        	// built-in module
            var example = CommonJS.require('path');
        	// relative to QML base url
            var example = CommonJS.require('./example.js');
        	// relative to this file
            var example = CommonJS.require(Qt.resolvedUrl('./example.js'));
        });
    }
}
```
