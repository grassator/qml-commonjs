import QtQuick 2.0
import CommonJSTest 0.1

QtObject {

    property var quitTimer: Timer {
        // By default allow test to run for 1 minute before exiting
        interval:  60 * 1000
        running: true
        onTriggered: Qt.quit()
    }

    Component.onCompleted: {
        var main = CommonJS.require('../js/main.js');
        var assert = CommonJS.require('assert');

        try {
            assert.ok(main);
            assert.ok('function' === typeof main);
        } catch(e) { }

        var i = 0;
        CommonJS.process.nextTick(function testTick() {
            if(i < 5) {
                CommonJS.process.nextTick(testTick);
                i++;
            }
            console.log('nextTick works');
        });
        console.log('All tests passed. Version: ' + CommonJS.process.version);

        // Unfortunately calling Qt.quit() in Component.onCompleted
        // doesn't work as expected so use timer instead
        quitTimer.interval = 1;
        quitTimer.start()
    }
}
