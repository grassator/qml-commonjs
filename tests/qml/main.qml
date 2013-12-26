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
            assert.ok(!main);
        } catch(e) { }

        // Unfortunately calling Qt.quit() in Component.onCompleted
        // doesn't work as expected so use timer instead
        quitTimer.interval = 1;
        quitTimer.start()
    }
}
