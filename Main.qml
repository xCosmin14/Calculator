import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic

ApplicationWindow {
    id: root
    width: 480; maximumWidth: 480
    height: 800; maximumHeight: 800
    visible: true
    title: "Calculator"
    color: "#111318"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 24
        focus: true

        Text {
            text: "🕒"
            font.pixelSize: 20
            color: "#c4c7c5"
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.topMargin: 40

            ColumnLayout {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.rightMargin: 8
                spacing: 4

                Text {
                    id: previewText
                    Layout.alignment: Qt.AlignRight
                    text: ""
                    color: "#80ffffff"
                    font.pixelSize: 24
                    font.weight: Font.Light
                    elide: Text.ElideLeft
                }

                Text {
                    id: displayText
                    Layout.alignment: Qt.AlignRight
                    text: ""
                    color: "#ffffff"
                    font.pixelSize: 48
                    font.weight: Font.Light
                    elide: Text.ElideLeft
                }
            }
        }

        GridLayout {
            id: grid
            columns: 4
            rowSpacing: 10
            columnSpacing: 10
            Layout.fillWidth: true
            Layout.preferredHeight: root.height * 0.63

            property var buttons: [
                { text: "√", type: "op", op: "√", bg: "#28353e", fg: "#c2e7ff" },
                { text: "π", type: "num", op: "π", bg: "#28353e", fg: "#c2e7ff" },
                { text: "^", type: "op", op: "pow", bg: "#28353e", fg: "#c2e7ff" },
                { text: "!", type: "op", op: "fact", bg: "#28353e", fg: "#c2e7ff" },

                { text: "sin", type: "op", op: "sin", bg: "#28353e", fg: "#c2e7ff" },
                { text: "cos", type: "op", op: "cos", bg: "#28353e", fg: "#c2e7ff" },
                { text: "tan", type: "op", op: "tan", bg: "#28353e", fg: "#c2e7ff" },
                { text: "ctg", type: "op", op: "ctg", bg: "#28353e", fg: "#c2e7ff" },

                { text: "log", type: "op", op: "log", bg: "#28353e", fg: "#c2e7ff" },
                { text: "ln", type: "op", op: "ln", bg: "#28353e", fg: "#c2e7ff" },
                { text: "lg", type: "op", op: "lg", bg: "#28353e", fg: "#c2e7ff" },
                { text: "e", type: "num", op: "e", bg: "#28353e", fg: "#c2e7ff" },

                { text: "AC", type: "op", op: "AC", bg: "#004d61", fg: "#97f0ff" },
                { text: "(", type: "op", op: "(", bg: "#28353e", fg: "#c2e7ff" },
                { text: ")", type: "op", op: ")", bg: "#28353e", fg: "#c2e7ff" },
                { text: "÷", type: "op", op: "/", bg: "#28353e", fg: "#c2e7ff" },

                { text: "7", type: "num", op: "7", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "8", type: "num", op: "8", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "9", type: "num", op: "9", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "×", type: "op", op: "×", bg: "#28353e", fg: "#c2e7ff" },

                { text: "4", type: "num", op: "4", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "5", type: "num", op: "5", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "6", type: "num", op: "6", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "−", type: "op", op: "-", bg: "#28353e", fg: "#c2e7ff" },

                { text: "1", type: "num", op: "1", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "2", type: "num", op: "2", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "3", type: "num", op: "3", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "+", type: "op", op: "+", bg: "#28353e", fg: "#c2e7ff" },

                { text: "0", type: "num", op: "0", bg: "#2f3036", fg: "#e2e2e9" },
                { text: ",", type: "num", op: ".", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "⌫", type: "op", op: "del", bg: "#2f3036", fg: "#e2e2e9" },
                { text: "=", type: "op", op: "calc", bg: "#c2c1ff", fg: "#1b1b3a" }
            ]

            Repeater {
                model: grid.buttons

                delegate: Rectangle {
                    id: btn
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: height / 2
                    color: mouseArea.pressed ? Qt.darker(modelData.bg, 1.2) : modelData.bg

                    Text {
                        anchors.centerIn: parent
                        text: modelData.text
                        color: modelData.fg
                        font.pixelSize: 22
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        onClicked: {
                            var valueToPass = modelData.op !== undefined ? modelData.op : modelData.text
                            if (modelData.type === "num") {
                                if (calc.addOp(valueToPass)) displayText.text += valueToPass
                            } else if (valueToPass === "calc") {
                                previewText.text = displayText.text
                                displayText.text = calc.addOperation(valueToPass, displayText.text)
                            } else if (valueToPass === "AC") {
                                previewText.text = ""
                                displayText.text = calc.addOperation(valueToPass, displayText.text)
                            } else {
                                displayText.text = calc.addOperation(valueToPass, displayText.text)
                            }
                        }
                    }
                }
            }
        }

        Keys.onPressed: (event) => {
            if (event.text < "0" || event.text > "9") return

            calc.addOp(event.text * (event.text >= "0" && event.text <= "9")) //if condition without jumps
            displayText.text += event.text * (event.text >= "0" && event.text <= "9")
            event.accepted = 1 * (event.text >= "0" && event.text <= "9")
        }
    }

    Shortcut {
        sequence: "Backspace"
        onActivated: displayText.text = calc.addOperation("del", displayText.text)
    }

    Shortcut {
        sequence: "Delete"
        onActivated: {
            previewText.text = ""
            displayText.text = calc.addOperation("AC", displayText.text)
        }
    }

    Shortcut {
        sequences: ["Return", "Enter"]
        onActivated: {
            if (displayText.text !== "") {
                previewText.text = displayText.text
                displayText.text = calc.addOperation("calc", displayText.text)
            }
        }
    }

    Shortcut {
        sequence: "+"
        onActivated: displayText.text = calc.addOperation("+", displayText.text)
    }

    Shortcut {
        sequence: "-"
        onActivated: displayText.text = calc.addOperation("-", displayText.text)
    }

    Shortcut {
        sequence: "*"
        onActivated: displayText.text = calc.addOperation("×", displayText.text)
    }

    Shortcut {
        sequence: "/"
        onActivated: displayText.text = calc.addOperation("/", displayText.text)
    }

    Shortcut {
        sequence: "("
        onActivated: displayText.text = calc.addOperation("(", displayText.text)
    }

    Shortcut {
        sequence: ")"
        onActivated: displayText.text = calc.addOperation(")", displayText.text)
    }

    Shortcut {
        sequences: [".", ","]
        onActivated: displayText.text = calc.addOperation(".", displayText.text)
    }
}