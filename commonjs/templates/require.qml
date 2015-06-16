import QtQuick 2.0;

QtObject {

    function requireInitializer(
        // this is a real parameter that is passed from C++
        __native
        // these parameters are here to disallow access to global objects
        // with these names from inside CommonJS. If it is discovered
        // that some of them _are_ useful inside CommonJS, then a
        // separate CommonJS module, (e.g. "qml") should be defined
        // to provide this functionality.
        , Qt, gc, print, XMLHttpRequest, qsTr, qsTranslate, qsTrId
        , QT_TR_NOOP, QT_TRANSLATE_NOOP, QT_TRID_NOOP
    ) {
        ['setTimeout', 'clearTimeout',
         'setInterval', 'clearInterval',
         'process'].forEach(function(key){
            __native.global[key] = __native[key];
        });

        // FIXME needs actual implementation
        function Buffer(){}

        var moduleSpecificLocals = ['__filename', '__dirname',
                                    'module', 'exports'];

        // Creating a function responsible for requiring modules
        var __require = function(moduleSpecificLocals, __filename, parentModule) {

            // Making sure we have fully resolved path
            // because it's later use in cache lookup
            __filename = __native.resolve(
                        __filename, parentModule ? parentModule.id : null);

            // if filename couldn't be resolved
            if(__filename instanceof Error) {
                throw __filename;
            }

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
            require.resolve = function(url, parent){
                var result = __native.resolve.apply(this, arguments);
                if(result instanceof Error) {
                    throw result;
                }
                return result;
            };

            // Making new require available in module object
            // http://nodejs.org/api/modules.html#modules_module_require_id
            module.require = require;

            // Adding to cache right away to deal with cyclic dependencies
            require.cache[__filename] = module;

            // creating a snippet of code that would expose registered
            // globals as local variables
            //
            // FIXME need to think of a way to mirror changes to local
            // vars into global and vice versa
            var globalFixSnippet = "";
            Object.keys(global).forEach(function(key){
                if(moduleSpecificLocals.indexOf(key) === -1) {
                    globalFixSnippet += "var " + key + " = global." + key + ";";
                }
            });

            // now in the same manner exposing module-specific
            // local vars as global object properties
            moduleSpecificLocals.forEach(function(key){
                globalFixSnippet += "global." + key + " = " + key +";";
            });

            // Applying fixes
            // @disable-check M23 // allowing eval
            eval(globalFixSnippet);

            // Cleaning namespace before code evaluation
            parentModule = undefined;
            moduleSpecificLocals = undefined;

            if(__filename.match(/\.json$/)) {
                module.exports = JSON.parse(__native.__loadFile(__filename));
            } else {
                try {
                    // Evaluating require'd code. Not using QJSEngine::evaluate
                    // to allow exceptions when requiring modules to naturally
                    // bubble up whereas QJSEngine::evaluate would simply return
                    // the error instead of bubbling it calling code because
                    // of necessary C++ calls.
                    //
                    // Additionally we have to wrap into an anonymous function
                    // because if code contains a top-level function who's name
                    // is the same as one of the properties on global object,
                    // e.g. hasOwnProperty, then it is considered a write to that
                    // property and thus results in an error.
                    //
                    // @disable-check M23 // allowing eval
                    eval("try{(function () {" +
                         __native.__loadFile(__filename) +
                         // there is probably a bug that will not propagate exception
                         // thrown inside eval() unless it's an instance of Error
                         "}())}catch(e){ throw e instanceof Error ? e : new Error(e); }"
                         );
                } catch(e) {
                    // Capturing stack trace. Unfortunately lineNumber is wrong with eval
                    e.message += '\n  ' + __filename /* + ':' + e.lineNumber */;
                    throw e;
                }
            }
            // Marking module as loaded
            module.loaded = true;

            // Providing user with the result
            return module.exports;
        };

        // Making sure __require is always executed in CommonJS
        // global context and not in QML global JS context
        __require = __require.bind(__native.global, moduleSpecificLocals);

        return __require;
    }

    function createFsStatsConstructor() {
        function Stats() {}

        Stats.prototype = {
            dev: 0,
            mode: 0,
            nlink: 0,
            uid: 0,
            gid: 0,
            rdev: 0,
            blksize: 0,
            ino: 0,
            size: 0,
            blocks: 0,
            atime: 0,
            mtime: 0,
            ctime: 0
        };

        return Stats;
    }

    function resolveInitializer(__native) {

        var builtInModules = ["_degugger", "_http_agent", "http_client",
                              "_http_common", "_http_incoming", "_http_outgoing",
                              "_http_server", "_linklist", "_stream_duplex",
                              "_stream_passthrough", "_stream_readable",
                              "_stream_transform", "_stream_writable",
                              "_tls_legacy", "_tls_wrap",
                              "assert", "buffer", "child_process", "cluster",
                              "console", "constants", "crypto", "dgram", "dns",
                              "domain", "events", "freelist", "fs", "http",
                              "https", "module", "net", "os", "path",
                              "punycode", "querystring", "readline", "repl",
                              "smalloc", "stream", "string_decoder",
                              "sys", "timers", "tls", "tty", "url", "util",
                              "vm", "zlib"];

        function checkModuleAsDirectory(module) {
            if(module.slice(-3) === ".js" || module.slice(-5) === ".json") {
                return module;
            }

            var packagePath = module + "/package.json";
            var mainFileName = "index.js";
            var fs = __native.require('fs');
            if(fs.existsSync(packagePath)) {
                var json = JSON.parse(__native.__loadFile(packagePath));
                if(json.main) {
                    mainFileName = json.main;
                }
            }

            return module + "/" + mainFileName;
        }

        // Creating a function responsible for requiring modules
        var __resolve = function(module, basePath) {

            // removing trimming end slash, file:// prefix and reducing to basename
            basePath = basePath.replace(/^file:\/\/|\/$|\/[^.\/\\]+\.js(on)?$/gi, '');

            // Shortcurcuiting for built-in modules
            if(builtInModules.indexOf(module) > -1) {
                return ":lib/" + module + ".js";
            }

            var path = __native.require('path');
            var fs = __native.require('fs');

            // removing prefix from file urls if present
            if(module.slice(0, 7) === "file://") {
               module = module.slice(7);
            }


            // relative path
            if(module.slice(0, 2) === "./" || module.slice(0, 3) === "../") {
                module = path.normalize(basePath + "/" + module);
                module = checkModuleAsDirectory(module);
            } else {
                // recursive upwards search in node_modules folders
                if(module[0] !== "/" && module.slice(0, 2) !== ":/") {
                    var previousPath, currentPath;
                    while(previousPath !== basePath) {
                        currentPath = basePath + "/node_modules/" + module;
                        currentPath = checkModuleAsDirectory(currentPath);
                        if(currentPath !== module && fs.existsSync(currentPath)) {
                            module = currentPath;
                            break;
                        }
                        previousPath = basePath;
                        basePath = path.normalize(basePath + '/..');
                    }
                } else {
                    module = checkModuleAsDirectory(module);
                }
            }

            if(!fs.existsSync(module)) {
                return "";
            }

            return module;
        };

        // Making sure __resolve is always executed in CommonJS
        // global context and not in QML global JS context
        __resolve = __resolve.bind(__native.global);

        return __resolve;
    }
}
