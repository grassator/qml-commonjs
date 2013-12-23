function doRequire(resolvedUrl) {
    // first checking cache
    var cached = CommonJS.__cachedRequire(resolvedUrl);
    if(typeof(cached) !== 'undefined') {
        return cached;
    }

    // Setting up expected global variables
    var module = { exports: {} };
    var exports = module.exports;

    // for nested require calls we need to adjust
    // base path so that relative require calls work
    var require = (function(base, url) {
        url = CommonJS.resolvedUrl(url, base);
        return doRequire(url);
    }).bind(this, resolvedUrl);

    // Evaluating require'd code. It would be better
    // to use QJSEngine::evaluate but unfortunately
    // it doesn't support nested calls
    var code = CommonJS.__loadFile(resolvedUrl);

    // @disable-check M23
    eval(code);

    // Adding to cache
    CommonJS.__addCachedRequire(resolvedUrl, module.exports);

    // Providing user with the result
    return module.exports;
}

// bootstrapping
doRequire('%1');
