// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import "qrc:/qml/styles" as Styles
import "qrc:/qml/components" as Components

Rectangle {
    id: delegateItem

    implicitWidth: treeView.width
    implicitHeight: label.implicitHeight * 2

    readonly property real indent: 20
    readonly property real padding: 5
    property bool isEditing: false

    // 暴露编辑框给其他 delegate 实例访问
    property alias editField: editTextField

    // TreeView 注入的必需属性
    required property TreeView treeView
    required property bool isTreeNode
    required property bool expanded
    required property int hasChildren
    required property int depth
    required property int row

    // 外部依赖：根窗口引用
    property var rootWindow: null

    // 当 isEditing 改变时，更新全局 editingDelegate
    onIsEditingChanged: {
        if (isEditing) {
            // 如果有其他项目正在编辑，先结束它
            if (rootWindow.editingDelegate && rootWindow.editingDelegate !== delegateItem && rootWindow.editingDelegate.isEditing) {
                rootWindow.editingDelegate.isEditing = false
            }
            rootWindow.editingDelegate = delegateItem
        } else if (rootWindow.editingDelegate === delegateItem) {
            rootWindow.editingDelegate = null
        }
    }

    color: {
        if (delegateItem.treeView.currentRow === row) {
            return Styles.Style.selectColor
        } else if (delegateMouseArea.containsMouse) {
            return Styles.Style.hoverColor
        } else {
            return "transparent"
        }
    }

    border.color: "transparent"
    border.width: 1
    radius: Styles.Style.borderRadius

    // 结束其他 delegate 的编辑状态
    function finishOtherEditing() {
        if (rootWindow.editingDelegate && rootWindow.editingDelegate !== delegateItem && rootWindow.editingDelegate.isEditing) {
            if (rootWindow.editingDelegate.editField && rootWindow.editingDelegate.editField.text.trim() !== "") {
                var editingIndex = rootWindow.editingDelegate.getCurrentIndex()
                treeView.model.renameItem(editingIndex, rootWindow.editingDelegate.editField.text.trim())
                menuManager.saveCurrentModel()
            }
            rootWindow.editingDelegate.isEditing = false
        }
    }

    // 获取当前项的索引（使用ID）
    function getCurrentIndex() {
        return treeView.model.getIndex(model.id || "")
    }

    // 递归展开/折叠指定节点的所有子孙节点
    function setExpandedRecursive(index, expand) {
        if (!index.valid) {
            return
        }

        // 获取视图中的行号
        var viewRow = treeView.rowAtIndex(index)
        if (viewRow >= 0) {
            if (expand) {
                treeView.expand(viewRow)
            } else {
                treeView.collapse(viewRow)
            }
        }

        // 递归处理所有子节点
        var rowCount = treeView.model.rowCount(index)
        for (var i = 0; i < rowCount; i++) {
            var childIndex = treeView.model.index(i, 0, index)
            setExpandedRecursive(childIndex, expand)
        }
    }

    // 检查指定节点的所有子孙节点是否都已展开（不包括当前节点本身）
    function areAllDescendantsExpanded(index) {
        if (!index.valid) {
            return true
        }

        // 递归检查所有子节点
        var rowCount = treeView.model.rowCount(index)
        for (var i = 0; i < rowCount; i++) {
            var childIndex = treeView.model.index(i, 0, index)
            var viewRow = treeView.rowAtIndex(childIndex)
            // 如果子节点未展开，返回false
            if (viewRow >= 0 && !treeView.isExpanded(viewRow)) {
                return false
            }
            // 递归检查子节点的后代
            if (!areAllDescendantsExpanded(childIndex)) {
                return false
            }
        }

        return true
    }

    // 检查指定节点的所有子孙节点是否都已折叠（不包括当前节点本身）
    function areAllDescendantsCollapsed(index) {
        if (!index.valid) {
            return true
        }

        // 递归检查所有子节点
        var rowCount = treeView.model.rowCount(index)
        for (var i = 0; i < rowCount; i++) {
            var childIndex = treeView.model.index(i, 0, index)
            var viewRow = treeView.rowAtIndex(childIndex)
            // 如果子节点已展开，返回false
            if (viewRow >= 0 && treeView.isExpanded(viewRow)) {
                return false
            }
            // 递归检查子节点的后代
            if (!areAllDescendantsCollapsed(childIndex)) {
                return false
            }
        }

        return true
    }

    // 右键菜单
    Menu {
        id: contextMenu

        Menu {
            title: qsTr("Add")

            MenuItem {
                text: qsTr("Add Sibling Menu")
                onTriggered: {
                    console.log("Add sibling menu for:", model.name)
                    rootWindow.forceSaveExecCommand()
                    var index = getCurrentIndex()
                    treeView.model.addSiblingItem(index, qsTr("New Menu"))
                    editTimer.start()
                }
            }

            MenuItem {
                text: qsTr("Add Child Menu")
                enabled: model.level < 3
                onTriggered: {
                    console.log("Add child menu for:", model.name)
                    rootWindow.forceSaveExecCommand()
                    var index = getCurrentIndex()
                    treeView.model.addChildItem(index, qsTr("New Menu"))
                    treeView.expand(row)
                    editTimer.start()
                }
            }
        }

        MenuItem {
            text: qsTr("Rename")
            onTriggered: {
                console.log("Rename menu:", model.name)
                delegateItem.isEditing = true
                editTextField.forceActiveFocus()
                editTextField.selectAll()
            }
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Delete")
            enabled: !model.isSystem
            onTriggered: {
                console.log("Delete menu:", model.name)
                rootWindow.forceSaveExecCommand()
                var index = getCurrentIndex()
                treeView.model.removeItem(index)
                menuManager.saveCurrentModel()
            }
        }

        MenuSeparator {}

        MenuItem {
            text: qsTr("Expand")
            enabled: delegateItem.hasChildren > 0 && !delegateItem.expanded
            onTriggered: {
                console.log("Expand node:", model.name)
                treeView.expand(row)
            }
        }

        MenuItem {
            text: qsTr("Collapse")
            enabled: delegateItem.hasChildren > 0 && delegateItem.expanded
            onTriggered: {
                console.log("Collapse node:", model.name)
                treeView.collapse(row)
            }
        }

        MenuItem {
            text: qsTr("Expand All")
            enabled: delegateItem.hasChildren > 0 && !delegateItem.expanded && !areAllDescendantsExpanded(getCurrentIndex())
            onTriggered: {
                console.log("Expand all from:", model.name)
                var index = getCurrentIndex()
                setExpandedRecursive(index, true)
            }
        }

        MenuItem {
            text: qsTr("Collapse All")
            enabled: delegateItem.hasChildren > 0 && delegateItem.expanded && !areAllDescendantsCollapsed(getCurrentIndex())
            onTriggered: {
                console.log("Collapse all from:", model.name)
                var index = getCurrentIndex()
                setExpandedRecursive(index, false)
            }
        }
    }

    // 定时器，用于延迟进入编辑模式（等待模型更新）
    Timer {
        id: editTimer
        interval: 100
        onTriggered: {
            delegateItem.isEditing = true
            editTextField.forceActiveFocus()
            editTextField.selectAll()
        }
    }

    // 展开/折叠按钮
    Text {
        id: indicator
        visible: delegateItem.isTreeNode && delegateItem.hasChildren > 0
        x: delegateItem.padding + (delegateItem.depth * delegateItem.indent)
        width: delegateItem.indent
        height: delegateItem.height
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        text: delegateItem.expanded ? "▼" : "▶"
        font: Styles.Style.tagFont
        color: Styles.Style.secondaryTextColor

        TapHandler {
            onTapped: {
                finishOtherEditing()

                // 记录展开状态变化
                var newExpandedState = !delegateItem.expanded
                var itemId = model.id || ""
                if (itemId !== "") {
                    if (newExpandedState) {
                        // 展开：添加到列表
                        if (rootWindow.savedExpandedItemIds.indexOf(itemId) < 0) {
                            rootWindow.savedExpandedItemIds.push(itemId)
                        }
                    } else {
                        // 折叠：从列表移除
                        var idx = rootWindow.savedExpandedItemIds.indexOf(itemId)
                        if (idx >= 0) {
                            rootWindow.savedExpandedItemIds.splice(idx, 1)
                        }
                    }
                }

                treeView.toggleExpanded(row)
                console.log("Toggle expand:", model.name, "expanded:", newExpandedState, "hasChildren:", delegateItem.hasChildren)
            }
        }
    }

    // 文本内容（非编辑模式）
    Text {
        id: label
        visible: !delegateItem.isEditing
        x: delegateItem.padding + (delegateItem.isTreeNode ? (delegateItem.depth + 1) * delegateItem.indent : delegateItem.depth * delegateItem.indent)
        width: delegateItem.width - delegateItem.padding - x
        height: delegateItem.height
        verticalAlignment: Text.AlignVCenter
        text: model.nameLocal || model.name || ""
        font.pixelSize: Styles.Style.bodyFont.pixelSize
        font.family: Styles.Style.bodyFont.family
        font.bold: delegateItem.treeView.currentRow === row
        color: Styles.Style.textColor
        elide: Text.ElideRight

        TapHandler {
            onTapped: {
                console.log("Clicked item:", model.name, "nameLocal:", model.nameLocal, "depth:", delegateItem.depth, "hasChildren:", delegateItem.hasChildren)
                finishOtherEditing()
                rootWindow.forceSaveExecCommand()
                rootWindow.currentItem = model
            }

            onDoubleTapped: {
                console.log("Double clicked item:", model.name)
                if (!model.isSystem) {
                    delegateItem.isEditing = true
                    editTextField.forceActiveFocus()
                    editTextField.selectAll()
                }
            }
        }
    }

    // 编辑模式下的TextField
    Components.DTextField {
        id: editTextField
        visible: delegateItem.isEditing
        x: delegateItem.padding + (delegateItem.isTreeNode ? (delegateItem.depth + 1) * delegateItem.indent : delegateItem.depth * delegateItem.indent)
        width: delegateItem.width - delegateItem.padding - x - 10
        height: delegateItem.height - 4
        anchors.verticalCenter: parent.verticalCenter
        text: model.nameLocal || model.name || ""

        onAccepted: {
            console.log("Edit accepted, new name:", text)
            if (text.trim() !== "") {
                var index = getCurrentIndex()
                treeView.model.renameItem(index, text.trim())
                menuManager.saveCurrentModel()
            }
            delegateItem.isEditing = false
        }

        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                console.log("Enter pressed, new name:", text)
                if (text.trim() !== "") {
                    var index = getCurrentIndex()
                    treeView.model.renameItem(index, text.trim())
                    menuManager.saveCurrentModel()
                }
                delegateItem.isEditing = false
                event.accepted = true
            } else if (event.key === Qt.Key_Escape) {
                console.log("Escape pressed, cancel edit")
                delegateItem.isEditing = false
                event.accepted = true
            }
        }

        onFocusChanged: {
            if (!focus && delegateItem.isEditing) {
                console.log("Focus lost, save edit")
                if (text.trim() !== "") {
                    var index = getCurrentIndex()
                    treeView.model.renameItem(index, text.trim())
                    menuManager.saveCurrentModel()
                }
                delegateItem.isEditing = false
            }
        }
    }

    // 右键菜单触发器
    MouseArea {
        id: delegateMouseArea
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        propagateComposedEvents: true
        hoverEnabled: true

        onClicked: function(mouse) {
            // 如果模型为空或没有项目，不处理右键点击，让事件传递给背景区域的MouseArea
            if (treeView.model === null || treeView.model.rowCount() === 0) {
                mouse.accepted = false
                return
            }

            finishOtherEditing()
            rootWindow.forceSaveExecCommand()
            rootWindow.currentItem = model
            contextMenu.popup(mouse)
            mouse.accepted = true
        }
    }
}
