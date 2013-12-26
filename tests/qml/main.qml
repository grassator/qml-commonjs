import QtQuick 2.0
import CommonJSTest 0.1

QtObject {

    property var quitTimer: Timer {
        interval: 1
        onTriggered: Qt.quit()
    }

    Component.onCompleted: {
        var main = CommonJS.require('../js/main.js');
        console.log(JSON.stringify(main));

        // Unfortunately calling Qt.quit() in Component.onCompleted
        // doesn't work as expected so use timer instead
        quitTimer.start()
    }
}
