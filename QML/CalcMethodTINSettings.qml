import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.1

ColumnLayout {

    ParamSetup {
        paramName: "TIN edge length limit:"

        SpinBoxCustom {
            id: maxTriEdgeLengthSpinBox
            from: 0
            to: 100
            stepSize: 2
            value: 10
            onValueChanged: Settings3DController.changeMaxTriangulationLineLength(value)
        }

    }
}
