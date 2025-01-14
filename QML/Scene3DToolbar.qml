import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12


Item{

    Rectangle{
        color: "#915181"
        Row{
            id: row

            Button{
                id: setCameraIsometricView
                width: 40
                height: 40
                icon.source: "./3dcube.svg"

                icon.color: theme.textColor

                background: Rectangle {
                    radius: 1
                    height: parent.height
                    width: parent.width
                    color: parent.checked ? theme.controlBorderColor : theme.menuBackColor
                    border.color: theme.controlBorderColor
                    border.width: 1
                }

                onClicked: Scene3dToolBarController.onSetCameraIsometricViewButtonClicked()
            }

            Button{
                id: fitAllinViewButton
                width: 40
                height: 40
                icon.source: "./fit-in-view.svg"

                icon.color: theme.textColor

                background: Rectangle {
                    radius: 1
                    height: parent.height
                    width: parent.width
                    color: parent.checked ? theme.controlBorderColor : theme.menuBackColor
                    border.color: theme.controlBorderColor
                    border.width: 1
                }

                onClicked: Scene3dToolBarController.onFitAllInViewButtonClicked()
            }

            Row{
                id: toolButtonsLayout
                Button{
                    id: selectionToolButton
                    width: 40
                    height: 40
                    checkable: true
                    icon.source: "./one-finger.svg"

                    icon.color: theme.textColor

                    background: Rectangle {
                        radius: 1
                        height: parent.height
                        width: parent.width
                        color: parent.checked ? theme.controlBorderColor : theme.menuBackColor
                        border.color: theme.controlBorderColor
                        border.width: 1
                    }

                    onCheckedChanged: Scene3dToolBarController.onBottomTrackVertexEditingModeButtonChecked(checked)
                }

                Button{
                    id: bottomTrackVertexComboSelectionToolButton
                    width: 40
                    height: 40
                    checkable: true
                    icon.source: "./combo-selection.svg"
                    icon.color: theme.textColor

                    background: Rectangle {
                        radius: 1
                        height: parent.height
                        width: parent.width
                        color: parent.checked ? theme.controlBorderColor : theme.menuBackColor
                        border.color: theme.controlBorderColor
                        border.width: 1
                    }

                    onCheckedChanged: Scene3dToolBarController.onBottomTrackVertexComboSelectionModeButtonChecked(checked)
                }
            }
        }

        ButtonGroup{
            property bool buttonChangeFlag : false
            id: buttonGroup
            buttons: toolButtonsLayout.children
            onCheckedButtonChanged: buttonChangeFlag = true
            onClicked: {
                if(!buttonChangeFlag)
                    checkedButton = null

                buttonChangeFlag = false;
            }
        }
    }


}



