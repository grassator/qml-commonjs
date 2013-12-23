function doRequire(__filename, base) {

    __filename = CommonJS.resolve(__filename, base);

    // first checking cache
    if(CommonJS.cache[__filename]) {
        return CommonJS.cache[__filename].exports;
    }

    // Setting up expected global variables
    // except for 'require', 'process' and Buffer
    // that require special treatment
    var __dirname = __filename.replace(/\/[^\/]*$/, '');
    var global = CommonJS.global;
    var module = { exports: {} };
    var exports = module.exports;

    // for nested require calls we need to adjust
    // base path so that relative require calls work
    var require = (function(base, url) {
        return doRequire.call(global, url, base);
    }).bind(global, __filename);

    // Providing require globals described here:
    // http://nodejs.org/api/globals.html#globals_require
    require.cache = CommonJS.cache;
    require.resolve = CommonJS.resolve;

    // Adding to cache right away to
    // deal with cyclic dependencies
    require.cache[__filename] = module;

    // Evaluating require'd code. It would be better
    // to use QJSEngine::evaluate but unfortunately
    // it doesn't support nested calls
    //
    // @disable-check M23
    eval(CommonJS.__loadFile(__filename));

    // Providing user with the result
    return module.exports;
}
