import QtQuick 2.0
import CommonJS 0.1

QtObject {

    property var quitTimer: Timer {
        // By default allow test to run for 1 minute before exiting
        interval:  1
        running: false
        onTriggered: {
            Qt.quit();
        }
    }

    Component.onCompleted: {
        CommonJS.require('../js/bootstrap.js');
        quitTimer.start();
    }
}
