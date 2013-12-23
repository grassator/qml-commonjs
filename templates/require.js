function doRequire(__filename, parentModule) {

    // Making sure we have fully resolved path
    // because it's later use in cache lookup
    __filename = CommonJS.resolve(__filename, parentModule ? parentModule.id : null);

    // first checking cache
    if(CommonJS.cache[__filename]) {
        return CommonJS.cache[__filename].exports;
    }

    // Setting up expected global variables
    // except for 'require', 'process' and Buffer
    // that require special treatment
    var __dirname = __filename.replace(/\/[^\/]*$/, '');
    var global = CommonJS.global;
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
        return doRequire.call(global, url, module);
    }).bind(global, module);

    // Providing require globals described here:
    // http://nodejs.org/api/globals.html#globals_require
    require.cache = CommonJS.cache;
    require.resolve = CommonJS.resolve;

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
    eval(CommonJS.__loadFile(__filename));

    // Marking module as loaded
    module.loaded = true;

    // Providing user with the result
    return module.exports;
}
