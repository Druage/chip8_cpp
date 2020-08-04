import QtQuick 2.7
import QtQuick.Controls 1.1
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import Chip8 1.0

ApplicationWindow {
    visible: true
    width: 1000
    height: 500
    title: qsTr("Chip8")
    color: "black";

    RenderSurface {
        id: surface;
        focus: true;
        anchors.fill: parent;
    }

}