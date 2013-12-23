function doRequire(resolvedUrl) {

    // first checking cache
    var cachedModule = CommonJS.cache[resolvedUrl];
    if(cachedModule) {
        return cachedModule.exports;
    }

    // Setting up expected global variables
    var module = { exports: {} };
    var exports = module.exports;

    // Adding to cache right away to
    // deal with cyclic dependencies
    CommonJS.cache[resolvedUrl] = module;

    // for nested require calls we need to adjust
    // base path so that relative require calls work
    var require = (function(base, url) {
        url = CommonJS.resolve(url, base);
        return doRequire(url);
    }).bind(this, resolvedUrl);

    // Providing require globals described here:
    // http://nodejs.org/api/globals.html#globals_require
    require.cache = CommonJS.cache;
    require.resolve = CommonJS.resolve;

    // Evaluating require'd code. It would be better
    // to use QJSEngine::evaluate but unfortunately
    // it doesn't support nested calls
    var code = CommonJS.__loadFile(resolvedUrl);

    // @disable-check M23
    eval(code);

    // Providing user with the result
    return module.exports;
}

// bootstrapping
doRequire('%1');
