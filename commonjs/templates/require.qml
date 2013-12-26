import QtQuick 2.0;

QtObject {

    function sandbox(
        // this is a real parameter that is passed from C++
        __native,
        // these parameters are here to disallow access to global objects
        // with these names from inside CommonJS. If it is discovered
        // that some of them _are_ useful inside CommonJS, then a
        // separate CommonJS module, (e.g. "qml") should be defined
        // to provide this functionality.
        Qt, gc, print, XMLHttpRequest, qsTr, qsTranslate, qsTrId,
        QT_TR_NOOP, QT_TRANSLATE_NOOP, QT_TRID_NOOP
    ) {

        // Creating a function responsible for requiring modules
        var __require = function(__filename, parentModule) {

            // Making sure we have fully resolved path
            // because it's later use in cache lookup
            __filename = __native.resolve(
                        __filename, parentModule ? parentModule.id : null);

            // first checking cache
            if(__native.cache[__filename]) {
                return __native.cache[__filename].exports;
            }

            // Setting up expected global variables
            // except for 'require', 'process' and Buffer
            // that require special treatment
            var __dirname = __filename.replace(/\/[^\/]*$/, '');
            var global = __native.global;
            var exports = {};
            var module = {
                id: __filename,
                exports: exports,
                loaded: false,
                parent: parentModule,
                children: []
            };

            // Adding this module as a child of parent modules
            if(parentModule) {
                parentModule.children.push(module);
            }

            // for nested require calls we need to adjust
            // base path so that relative require calls work
            var require = (function(module, url) {
                return __require.call(global, url, module);
            }).bind(global, module);

            // Providing require globals described here:
            // http://nodejs.org/api/globals.html#globals_require
            require.cache = __native.cache;
            require.resolve = __native.resolve;

            // Making new require available in module object
            // http://nodejs.org/api/modules.html#modules_module_require_id
            module.require = require;

            // Adding to cache right away to
            // deal with cyclic dependencies
            require.cache[__filename] = module;

            // Cleaning namespace before code evaluation
            parentModule = undefined;

            // Evaluating require'd code. It would be better
            // to use QJSEngine::evaluate but unfortunately
            // it doesn't support nested calls
            //
            // @disable-check M23
            eval(__native.__loadFile(__filename));

            // Marking module as loaded
            module.loaded = true;

            // Providing user with the result
            return module.exports;
        };

        // Making sure __require is always executed in CommonJS
        // global context and not in QML global JS context
        __require = __require.bind(__native.global);

        return __require;
    }
}
