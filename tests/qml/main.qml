import QtQuick 2.0
import CommonJS 0.2

QtObject {

    Component.onCompleted: {
        CommonJS.require('./js/bootstrap.js');
        var i = 1;
        var intervalId = CommonJS.setInterval(function(){
            console.log('Interval call ' + i);
            if(++i > 5) {
               CommonJS.clearInterval(intervalId);
            }
        }, 10);
        CommonJS.setTimeout(function(){
            Qt.quit();
        }, 100);
    }
}
