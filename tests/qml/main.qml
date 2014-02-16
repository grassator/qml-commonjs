import QtQuick 2.0
import CommonJS 0.1

QtObject {

    Component.onCompleted: {
        CommonJS.require('../js/bootstrap.js');
        CommonJS.setTimeout(function(){
            Qt.quit();
        }, 10);
    }
}
