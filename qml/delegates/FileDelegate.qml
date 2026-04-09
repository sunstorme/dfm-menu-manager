// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import "qrc:/qml/styles" as Styles
import "qrc:/qml/components" as Components

Rectangle {
    id: fileDelegateItem

    width: ListView.view ? ListView.view.width : 100
    height: Styles.Style.itemHeight

    // 外部依赖
    property var rootWindow: null
    property var userFileModelRef: null
    property var systemFileModelRef: null
    property var fileContextMenu: null

    color: {
        if (model.filePath === rootWindow.selectedFilePath) {
            return Styles.Style.selectColor
        } else if (mouseArea.containsMouse) {
            return Styles.Style.hoverColor
        } else {
            return "transparent"
        }
    }
    radius: Styles.Style.borderRadius

    // 判断当前文件所属的模型
    function getCurrentModel() {
        if (model.filePath && model.filePath.indexOf("/.local/share/") !== -1) {
            return userFileModelRef
        } else if (model.filePath && model.filePath.indexOf("/usr/share/") !== -1) {
            return systemFileModelRef
        }
        return null
    }

    // 文件名显示或编辑框
    Loader {
        id: fileNameLoader
        anchors.left: parent.left
        anchors.leftMargin: Styles.Style.padding
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: Styles.Style.padding + (model.isSystem ? 50 : 0)

        // 判断是否应该显示编辑框
        property bool shouldShowEdit: {
            // 新建文件的占位符项（路径为空）
            if (rootWindow.isNewFile && model.filePath === "" && rootWindow.editingModelRef) {
                return true
            }
            // 重命名现有文件
            if (!rootWindow.isNewFile && rootWindow.editingFilePath === model.filePath) {
                return true
            }
            return false
        }

        sourceComponent: shouldShowEdit ? editComponent : textComponent
    }

    Component {
        id: textComponent
        Text {
            text: model.fileName || ""
            font.pixelSize: Styles.Style.bodyFont.pixelSize
            font.family: Styles.Style.bodyFont.family
            font.bold: model.filePath === rootWindow.selectedFilePath
            color: Styles.Style.textColor
            elide: Text.ElideRight
        }
    }

    Component {
        id: editComponent
        TextInput {
            id: textInput
            text: {
                if (rootWindow.isNewFile) {
                    return ""
                } else {
                    // 移除 .conf 扩展名用于编辑
                    var fileName = model.fileName || ""
                    return fileName.replace(/\.conf$/, '')
                }
            }
            font.pixelSize: Styles.Style.bodyFont.pixelSize
            font.family: Styles.Style.bodyFont.family
            color: Styles.Style.textColor
            selectByMouse: true
            selectionColor: Styles.Style.primaryColor

            onAccepted: {
                finishEditing(text)
            }

            onEditingFinished: {
                if (!rootWindow.isNewFile && (rootWindow.editingFilePath === model.filePath || rootWindow.editingFilePath === "")) {
                    if (text.trim() !== "") {
                        finishEditing(text)
                    }
                }
            }

            Keys.onPressed: function(event) {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    event.accepted = true
                    finishEditing(text)
                } else if (event.key === Qt.Key_Escape) {
                    event.accepted = true
                    cancelEditing()
                }
            }

            onFocusChanged: {
                if (!focus && (fileNameLoader.shouldShowEdit)) {
                    console.log("TextInput focus lost, finishing edit")
                    finishEditing(text)
                }
            }

            Component.onCompleted: {
                forceActiveFocus()
                selectAll()
            }
        }
    }

    Text {
        anchors.right: parent.right
        anchors.rightMargin: Styles.Style.padding
        anchors.verticalCenter: parent.verticalCenter
        text: model.isSystem ? qsTr("System") : ""
        font: Styles.Style.tagFont
        color: Styles.Style.systemTagColor
        visible: model.isSystem
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        // 禁用点击，让Loader处理
        enabled: !fileNameLoader.shouldShowEdit

        onDoubleClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton && model.filePath !== "") {
                console.log("Double-clicked on file:", model.filePath)
                var currentModel = getCurrentModel()
                console.log("currentModel:", currentModel, "typeof:", typeof currentModel)
                rootWindow.editingFilePath = model.filePath
                rootWindow.isNewFile = false
                rootWindow.editingModelRef = currentModel
                console.log("Set editingModelRef to:", rootWindow.editingModelRef)
            }
        }

        onClicked: function(mouse) {
            if (mouse.button === Qt.LeftButton) {
                // 如果有其他文件正在编辑，先结束编辑
                if (rootWindow.editingFilePath !== "" && rootWindow.editingFilePath !== model.filePath) {
                    console.log("Another file is being edited, ending edit first")
                    rootWindow.editingFilePath = ""
                    rootWindow.isNewFile = false
                    rootWindow.editingModelRef = null
                }
                rootWindow.selectedFilePath = model.filePath || ""
                menuManager.setCurrentConfig(model.filePath || "")
                fileContextMenu.filePath = model.filePath || ""
            } else if (mouse.button === Qt.RightButton) {
                // 如果有其他文件正在编辑，先结束编辑
                if (rootWindow.editingFilePath !== "" && rootWindow.editingFilePath !== model.filePath) {
                    console.log("Another file is being edited, ending edit first for right-click")
                    rootWindow.editingFilePath = ""
                    rootWindow.isNewFile = false
                    rootWindow.editingModelRef = null
                }
                var currentModel = getCurrentModel()
                fileContextMenu.filePath = model.filePath || ""
                fileContextMenu.modelRef = currentModel
                console.log("Right-click: setting contextMenu.modelRef to", currentModel, "typeof:", typeof currentModel)
                fileContextMenu.popup()
            }
        }
    }

    // 完成编辑的函数
    function finishEditing(newName) {
        var trimmedName = newName.trim()

        if (trimmedName !== "") {
            if (rootWindow.isNewFile && model.filePath === "" && rootWindow.editingModelRef && rootWindow.editingModelRef.createFile) {
                // 创建新文件
                rootWindow.editingModelRef.createFile(trimmedName)
            } else if (!rootWindow.isNewFile && rootWindow.editingFilePath !== "" && rootWindow.editingModelRef && rootWindow.editingModelRef.renameFile) {
                // 重命名文件
                rootWindow.editingModelRef.renameFile(rootWindow.editingFilePath, trimmedName)
            }
        }

        cancelEditing()
    }

    // 取消编辑的函数
    function cancelEditing() {
        if (rootWindow.isNewFile && model.filePath === "" && rootWindow.editingModelRef && rootWindow.editingModelRef.cancelNewFile) {
            // 取消新建文件，移除占位符
            rootWindow.editingModelRef.cancelNewFile()
        }

        rootWindow.editingFilePath = ""
        rootWindow.isNewFile = false
        rootWindow.editingModelRef = null
    }
}
