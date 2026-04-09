// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DFMMenu 1.0
import "qrc:/qml/styles" as Styles
import "qrc:/qml/components" as Components
import "qrc:/qml/delegates" as Delegates
import "qrc:/qml/js/Utils.js" as Utils

ApplicationWindow {
    id: root
    visible: true
    width: 1400
    height: 900
    minimumWidth: 1000
    minimumHeight: 600
    title: qsTr("DFM Context Menu Manager")

    // 调试日志开关（生产环境应设置为 false）
    property bool debugLogging: false
    

    // 窗口状态管理
    Component.onCompleted: {
        console.log("=== Component.onCompleted called ===")
        WindowManager.restoreState(root, root)
    }
    
    Component.onDestruction: {
        WindowManager.saveState(root, filePanelWidth, menuEditorWidth, propertyPanelWidth)
    }

    // 窗口关闭时保存正在编辑的内容
    onClosing: function(close) {
        // 强制保存 suffixField 的编辑内容
        if (suffixField && suffixField.hasFocus && currentItem && currentMenuModel) {
            var suffixes = Utils.parseSuffixes(suffixField.currentText, ":")
            var index = currentMenuModel.getIndex(currentItem.id)
            currentMenuModel.updateItem(index, "supportSuffix", suffixes)
            menuManager.saveCurrentModel()
        }
        // 强制保存 execCommandField 的编辑内容
        forceSaveExecCommand()
    }

    // 主布局 - 使用SplitView实现可拖动分隔器
    Components.DSplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        
        // 左侧面板 - 文件管理区
        Rectangle {
            id: filePanel
            SplitView.minimumWidth: 250
            SplitView.preferredWidth: filePanelWidth
            color: Styles.Style.backgroundColor
            
            Column {
                anchors.fill: parent
                anchors.margins: Styles.Style.padding
                spacing: Styles.Style.spacing
                
                // 搜索框
                Components.DTextField {
                    id: searchBox
                    width: parent.width
                    height: Styles.Style.itemHeight
                    placeholderText: qsTr("Search configuration files...")
                    
                    onTextChanged: {
                        userFileModel.searchFilter = text
                        systemFileModel.searchFilter = text
                    }
                }
                
                ScrollView {
                    width: parent.width
                    height: parent.height - searchBox.height - Styles.Style.spacing
                    
                    ColumnLayout {
                        width: parent.width
                        spacing: Styles.Style.spacing
                        
                        // 用户配置文件区(可折叠)
                        Components.DCollapsibleGroup {
                            id: userGroupBox
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            title: qsTr("User Configuration")
                            expanded: userExpanded
                            onExpandedChanged: userExpanded = expanded

                            ListView {
                                width: userGroupBox.width - 2 * Styles.Style.padding - 2 * userGroupBox.border.width
                                height: userGroupBox.expanded ? contentHeight : 0
                                implicitHeight: contentHeight
                                model: MenuFileModel {
                                    id: userFileModel
                                    showSystemOnly: false
                                }
                                delegate: Delegates.FileDelegate {
                                    rootWindow: root
                                    userFileModelRef: userFileModel
                                    systemFileModelRef: systemFileModel
                                    fileContextMenu: contextMenu
                                }
                                clip: true

                                Keys.onPressed: function(event) {
                                    console.log("userList: Key pressed:", event.key)
                                    if (event.key === Qt.Key_F5) {
                                        console.log("F5 pressed, refreshing file lists")
                                        userFileModel.refresh()
                                        systemFileModel.refresh()
                                        event.accepted = true
                                    } else if (event.key === Qt.Key_Delete) {
                                        console.log("Delete key pressed, selectedFilePath:", selectedFilePath)
                                        if (selectedFilePath !== "") {
                                            var isSystemFile = selectedFilePath.indexOf("/usr/share/") !== -1
                                            if (!isSystemFile) {
                                                console.log("Deleting user file:", selectedFilePath)
                                                userFileModel.deleteFile(selectedFilePath)
                                            } else {
                                                console.log("Cannot delete system file")
                                            }
                                            event.accepted = true
                                        }
                                    }
                                }
                            }
                        }

                        // 系统配置文件区(可折叠)
                        Components.DCollapsibleGroup {
                            id: systemGroupBox
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            title: qsTr("System Configuration")
                            expanded: systemExpanded
                            onExpandedChanged: systemExpanded = expanded

                            ListView {
                                width: systemGroupBox.width - 2 * Styles.Style.padding - 2 * systemGroupBox.border.width
                                height: systemGroupBox.expanded ? contentHeight : 0
                                implicitHeight: contentHeight
                                model: MenuFileModel {
                                    id: systemFileModel
                                    showSystemOnly: true
                                }
                                delegate: Delegates.FileDelegate {
                                    rootWindow: root
                                    userFileModelRef: userFileModel
                                    systemFileModelRef: systemFileModel
                                    fileContextMenu: contextMenu
                                }
                                clip: true

                                Keys.onPressed: function(event) {
                                    console.log("systemList: Key pressed:", event.key)
                                    if (event.key === Qt.Key_F5) {
                                        console.log("F5 pressed, refreshing file lists")
                                        userFileModel.refresh()
                                        systemFileModel.refresh()
                                        event.accepted = true
                                    } else if (event.key === Qt.Key_Delete) {
                                        console.log("Delete key pressed in system list, but system files cannot be deleted")
                                        event.accepted = true
                                    }
                                }
                            }
                        }

                        // 根节点属性区(可折叠)
                        Components.DCollapsibleGroup {
                            id: rootPropertyGroup
                            Layout.fillWidth: true
                            title: qsTr("Configuration Properties")
                            expanded: rootConfigData !== null
                            visible: rootConfigData !== null

                            Column {
                                width: rootPropertyGroup.width - 2 * Styles.Style.padding - 2 * rootPropertyGroup.border.width
                                spacing: Styles.Style.spacing

                                Components.DPropertyField {
                                    width: parent.width
                                    labelText: qsTr("Description")
                                    propertyName: "comment"
                                    fieldValue: rootConfigData ? rootConfigData.comment || "" : ""
                                    onValueEdited: function(name, value) { updateProperty(name, value) }
                                }

                                Components.DPropertyField {
                                    width: parent.width
                                    labelText: qsTr("Description (Chinese)")
                                    propertyName: "commentLocal"
                                    fieldValue: rootConfigData ? rootConfigData.commentLocal || "" : ""
                                    onValueEdited: function(name, value) { updateProperty(name, value) }
                                }

                                Components.DPropertyField {
                                    width: parent.width
                                    labelText: qsTr("Version")
                                    propertyName: "version"
                                    fieldValue: rootConfigData ? rootConfigData.version || "" : ""
                                    onValueEdited: function(name, value) { updateProperty(name, value) }
                                }
                            }
                        }
                    }
                }
            }
            
            onWidthChanged: {
                filePanelWidth = width
            }
        }
        
        // 中间面板 - 菜单编辑区
        Rectangle {
            id: menuEditor
            SplitView.minimumWidth: 400
            SplitView.preferredWidth: menuEditorWidth
            color: Styles.Style.backgroundColor
            
            // 空配置文件提示
            Text {
                anchors.centerIn: parent
                text: selectedFilePath === "" ? qsTr("Please select a configuration file") :
                    currentMenuModel === null ? qsTr("Loading configuration...") : ""
                font: Styles.Style.bodyFont
                color: Styles.Style.secondaryTextColor
                // 只在未选择配置文件时显示提示
                // 当配置文件为空时，TreeView 应该显示，让用户可以右键添加项目
                visible: selectedFilePath === ""
                z: 1
            }

            Column {
                anchors.fill: parent
                anchors.margins: Styles.Style.padding
                spacing: 0
                
                // 工具栏
                Rectangle {
                    width: parent.width
                    height: Styles.Style.toolbarHeight
                    color: Styles.Style.backgroundColor
                    
                    Text {
                        anchors.left: parent.left
                        text: qsTr("Menu Structure Editor")
                        font: Styles.Style.h1Font
                        color: Styles.Style.textColor
                    }
                }
                
                // 菜单树视图
                Rectangle {
                    width: parent.width
                    height: parent.height - Styles.Style.toolbarHeight
                    color: Styles.Style.backgroundColor
                          
                    // 背景区域右键菜单（用于空配置文件）
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        propagateComposedEvents: true
                        z: 2
                        
                        onClicked: function(mouse) {
                            // 如果已选择配置文件，但模型为空或没有项目，显示添加菜单
                            if (selectedFilePath !== "" && (currentMenuModel === null || currentMenuModel.rowCount() === 0)) {
                                emptyContextMenu.popup(mouse)
                                mouse.accepted = true
                            } else {
                                // 其他情况让事件传递给delegate处理
                                mouse.accepted = false
                            }
                        }
                    }
                    
                    ScrollView {
                        id: menuScrollView
                        width: parent.width
                        anchors.fill: parent
                        
                        TreeView {
                            id: menuTreeView
                            width: menuScrollView.width
                            height: menuScrollView.height
                            model: currentMenuModel
                            delegate: Delegates.TreeDelegate {
                                rootWindow: root
                            }
                            clip: true
                            alternatingRows: true
                            
                            selectionModel: ItemSelectionModel {
                                id: treeSelection
                            }
                        }
                    }
                    
                    // 空配置文件的右键菜单
                    Menu {
                        id: emptyContextMenu
                        
                        MenuItem {
                            text: qsTr("Add Menu")
                            onTriggered: {
                                if (currentMenuModel !== null) {
                                    // 强制保存 execCommandField 的编辑内容
                                    forceSaveExecCommand()
                                    var rootIndex = currentMenuModel.index(0, 0)
                                    currentMenuModel.addChildItem(rootIndex, qsTr("New Menu"))
                                    // 保存到文件
                                    menuManager.saveCurrentModel()
                                }
                            }
                        }
                    }
                }
            }
            
            onWidthChanged: {
                menuEditorWidth = width
            }
        }
        
        // 右侧面板 - 属性编辑区
        Rectangle {
            id: propertyPanel
            SplitView.minimumWidth: 300
            SplitView.preferredWidth: propertyPanelWidth
            color: Styles.Style.backgroundColor
            
            // 未选中任何项时的提示
            Text {
                visible: currentItem === null
                anchors.centerIn: parent
                text: qsTr("Please select a menu item")
                font: Styles.Style.bodyFont
                color: Styles.Style.secondaryTextColor
            }

            ColumnLayout{
                anchors.fill: parent
                anchors.margins: Styles.Style.padding
                spacing: Styles.Style.spacing
                
                Text {
                    id: propertyTitle
                    Layout.fillWidth: true
                    text: currentItem ? qsTr("Property Editor: ") + (currentItem.nameLocal || currentItem.name || "") : qsTr("Property Editor")
                    font: Styles.Style.h1Font
                    color: Styles.Style.textColor
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth
                    contentHeight: column.implicitHeight
                    
                    Column {
                        id: column
                        width: parent.width
                        spacing: Styles.Style.spacing * 2

                        // 菜单项属性
                        Column {
                            visible: currentItem !== null && currentItem.level > 0
                            width: parent.width
                            spacing: Styles.Style.spacing
                            
                            // Name
                            Components.DPropertyField {
                                width: parent.width
                                labelText: qsTr("Menu Name")
                                propertyName: "name"
                                fieldValue: currentItem ? currentItem.name || "" : ""
                                onValueEdited: function(name, value) { updateProperty(name, value) }
                            }

                            // Name[zh_CN]
                            Components.DPropertyField {
                                width: parent.width
                                labelText: qsTr("Menu Name (Chinese)")
                                propertyName: "nameLocal"
                                fieldValue: currentItem ? currentItem.nameLocal || "" : ""
                                onValueEdited: function(name, value) { updateProperty(name, value) }
                            }
                            
                            // X-DFM-MenuTypes (checkbox)
                            Column {
                                width: parent.width
                                spacing: 5
                                
                                Text {
                                    text: qsTr("Menu Type")
                                    font: Styles.Style.h3Font
                                    color: Styles.Style.secondaryTextColor
                                }
                                
                                GridLayout {
                                    width: parent.width
                                    columns: 2
                                    rowSpacing: 5
                                    columnSpacing: 10
                                    
                                    Components.DCheckBox {
                                        id: singleFileCheckBox
                                        text: "SingleFile"
                                        checked: currentItem && currentItem.menuTypes ? currentItem.menuTypes.indexOf("SingleFile") >= 0 : false

                                        onClicked: toggleMenuType("SingleFile", checked)
                                    }
                                    Components.DCheckBox {
                                        id: multiFilesCheckBox
                                        text: "MultiFiles"
                                        checked: currentItem && currentItem.menuTypes ? currentItem.menuTypes.indexOf("MultiFiles") >= 0 : false

                                        onClicked: toggleMenuType("MultiFiles", checked)
                                    }
                                    Components.DCheckBox {
                                        id: filemanagerCheckBox
                                        text: "Filemanager"
                                        checked: currentItem && currentItem.menuTypes ? currentItem.menuTypes.indexOf("Filemanager") >= 0 : false

                                        onClicked: toggleMenuType("Filemanager", checked)
                                    }
                                    Components.DCheckBox {
                                        id: singleDirCheckBox
                                        text: "SingleDir"
                                        checked: currentItem && currentItem.menuTypes ? currentItem.menuTypes.indexOf("SingleDir") >= 0 : false

                                        onClicked: toggleMenuType("SingleDir", checked)
                                    }
                                    Components.DCheckBox {
                                        id: blankSpaceCheckBox
                                        text: "BlankSpace"
                                        checked: currentItem && currentItem.menuTypes ? currentItem.menuTypes.indexOf("BlankSpace") >= 0 : false

                                        onClicked: toggleMenuType("BlankSpace", checked)
                                    }
                                }
                            }
                                                    
                            // PosNum
                            Column {
                                width: parent.width
                                spacing: 5

                                Text {
                                    text: qsTr("Position Number")
                                    font: Styles.Style.h3Font
                                    color: Styles.Style.secondaryTextColor
                                }

                                Row {
                                    width: parent.width
                                    spacing: Styles.Style.spacing

                                    Column {
                                        width: (parent.width - parent.spacing * 2) / 3
                                        spacing: 3

                                        Text {
                                            text: qsTr("Default")
                                            font: Styles.Style.bodyFont
                                            color: Styles.Style.secondaryTextColor
                                        }

                                        Components.DSpinBox {
                                            id: positionSpinBox
                                            width: parent.width
                                            height: Styles.Style.itemHeight
                                            from: 1
                                            to: 100
                                            value: currentItem ? currentItem.positionNumber || 1 : 1

                                            Connections {
                                                target: root
                                                function onCurrentItemChanged() {
                                                    positionSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumber || 1 : 1
                                                    })
                                                }
                                            }

                                            onUserModified: {
                                                if (currentItem && currentMenuModel && originalItemValues) {
                                                    var index = currentMenuModel.getIndex(currentItem.id)
                                                    currentMenuModel.updateItem(index, "positionNumber", value)
                                                    originalItemValues.positionNumber = value
                                                    menuManager.saveCurrentModel()
                                                    positionSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumber || 1 : 1
                                                    })
                                                }
                                            }
                                        }
                                    }

                                    Column {
                                        width: (parent.width - parent.spacing * 2) / 3
                                        spacing: 3

                                        Text {
                                            text: qsTr("SingleFile")
                                            font: Styles.Style.bodyFont
                                            color: Styles.Style.secondaryTextColor
                                        }

                                        Components.DSpinBox {
                                            id: positionSingleFileSpinBox
                                            width: parent.width
                                            height: Styles.Style.itemHeight
                                            from: 1
                                            to: 100
                                            value: currentItem ? currentItem.positionNumberSingleFile || 1 : 1

                                            Connections {
                                                target: root
                                                function onCurrentItemChanged() {
                                                    positionSingleFileSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumberSingleFile || 1 : 1
                                                    })
                                                }
                                            }

                                            onUserModified: {
                                                if (currentItem && currentMenuModel && originalItemValues) {
                                                    var index = currentMenuModel.getIndex(currentItem.id)
                                                    currentMenuModel.updateItem(index, "positionNumberSingleFile", value)
                                                    originalItemValues.positionNumberSingleFile = value
                                                    menuManager.saveCurrentModel()
                                                    positionSingleFileSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumberSingleFile || 1 : 1
                                                    })
                                                }
                                            }
                                        }
                                    }

                                    Column {
                                        width: (parent.width - parent.spacing * 2) / 3
                                        spacing: 3

                                        Text {
                                            text: qsTr("MultiFiles")
                                            font: Styles.Style.bodyFont
                                            color: Styles.Style.secondaryTextColor
                                        }

                                        Components.DSpinBox {
                                            id: positionMultiFilesSpinBox
                                            width: parent.width
                                            height: Styles.Style.itemHeight
                                            from: 1
                                            to: 100
                                            value: currentItem ? currentItem.positionNumberMultiFiles || 1 : 1

                                            Connections {
                                                target: root
                                                function onCurrentItemChanged() {
                                                    positionMultiFilesSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumberMultiFiles || 1 : 1
                                                    })
                                                }
                                            }

                                            onUserModified: {
                                                if (currentItem && currentMenuModel && originalItemValues) {
                                                    var index = currentMenuModel.getIndex(currentItem.id)
                                                    currentMenuModel.updateItem(index, "positionNumberMultiFiles", value)
                                                    originalItemValues.positionNumberMultiFiles = value
                                                    menuManager.saveCurrentModel()
                                                    positionMultiFilesSpinBox.value = Qt.binding(function() {
                                                        return currentItem ? currentItem.positionNumberMultiFiles || 1 : 1
                                                    })
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            
                            // Separator
                            Column {
                                width: parent.width
                                spacing: 5
                                
                                Text {
                                    text: qsTr("Separator")
                                    font: Styles.Style.h3Font
                                    color: Styles.Style.secondaryTextColor
                                }
                                
                                Row {
                                    width: parent.width
                                    spacing: Styles.Style.spacing * 4
                                    
                                    Components.DRadioButton {
                                        id: separatorTopRadio
                                        text: qsTr("Top")
                                        checked: currentItem && currentItem.separator === "Top"
                                        
                                        onClicked: {
                                            if (currentItem && currentMenuModel && originalItemValues) {
                                                // 直接更新模型并保存
                                                var index = currentMenuModel.getIndex(currentItem.id)
                                                currentMenuModel.updateItem(index, "separator", "Top")
                                                // 更新原始值以避免重复保存
                                                originalItemValues.separator = "Top"
                                                // 保存到文件
                                                menuManager.saveCurrentModel()
                                            }
                                        }
                                    }
                                    
                                    Components.DRadioButton {
                                        id: separatorBottomRadio
                                        text: qsTr("Bottom")
                                        checked: currentItem && currentItem.separator === "Bottom"

                                        onClicked: {
                                            if (currentItem && currentMenuModel && originalItemValues) {
                                                // 直接更新模型并保存
                                                var index = currentMenuModel.getIndex(currentItem.id)
                                                currentMenuModel.updateItem(index, "separator", "Bottom")
                                                // 更新原始值以避免重复保存
                                                originalItemValues.separator = "Bottom"
                                                // 保存到文件
                                                menuManager.saveCurrentModel()
                                            }
                                        }
                                    }

                                    Components.DRadioButton {
                                        id: separatorNoneRadio
                                        text: qsTr("None")

                                        checked: currentItem && (!currentItem.separator || currentItem.separator === "")

                                        onClicked: {
                                            if (currentItem && currentMenuModel && originalItemValues) {
                                                var index = currentMenuModel.getIndex(currentItem.id)
                                                currentMenuModel.updateItem(index, "separator", "")
                                                originalItemValues.separator = ""
                                                menuManager.saveCurrentModel()
                                            }
                                        }
                                    }
                                }
                            }
                            
                            // X-DFM-SupportSuffix (多行输入框+选择按钮)
                            Column {
                                width: parent.width
                                spacing: 5

                                Text {
                                    text: qsTr("Supported Suffixes")
                                    font: Styles.Style.h3Font
                                    color: Styles.Style.secondaryTextColor
                                }

                                Row {
                                    width: parent.width
                                    height: Styles.Style.itemHeight * 3
                                    spacing: Styles.Style.spacing

                                    Components.DMultiLinePropertyField {
                                        id: suffixField
                                        width: parent.width - selectButton.width - parent.spacing
                                        fieldHeight: parent.height
                                        labelText: ""
                                        placeholderText: qsTr("Enter supported suffixes, separated by colons, e.g.: mp4:avi:mkv")
                                        fieldValue: {
                                            if (currentItem && currentItem.supportSuffix) {
                                                return currentItem.supportSuffix.join(":")
                                            }
                                            return ""
                                        }

                                        onValueEdited: function(value) {
                                            if (currentItem && currentMenuModel && originalItemValues) {
                                                var suffixes = Utils.parseSuffixes(value, ":")
                                                var index = currentMenuModel.getIndex(currentItem.id)
                                                currentMenuModel.updateItem(index, "supportSuffix", suffixes)
                                                originalItemValues.supportSuffix = suffixes.slice()
                                                menuManager.saveCurrentModel()
                                            }
                                        }
                                    }

                                    Components.DButton {
                                        id: selectButton
                                        width: Styles.Style.itemHeight * 2
                                        text: qsTr("Select")

                                        onClicked: {
                                            fileTypeSelectorDialog.open()
                                        }
                                    }
                                }
                            }
                            
                            // Exec (始终显示)
                            Components.DMultiLinePropertyField {
                                id: execCommandField
                                visible: currentItem !== null
                                width: parent.width
                                labelText: qsTr("Executable Command")
                                fieldValue: currentItem ? currentItem.execCommand || "" : ""

                                onValueEdited: function(value) {
                                    if (currentItem && currentMenuModel && originalItemValues) {
                                        var index = currentMenuModel.getIndex(currentItem.id)
                                        currentMenuModel.updateItem(index, "execCommand", value)
                                        originalItemValues.execCommand = value
                                        menuManager.saveCurrentModel()
                                    }
                                }
                            }
                        }
                    }
                    
                }
            }
            
            onWidthChanged: {
                propertyPanelWidth = width
            }
        }
    }
    
    // 折叠状态
    property bool userExpanded: true
    property bool systemExpanded: true
    
    // 当前选中的文件路径
    property string selectedFilePath: ""

    // 当前菜单树模型
    property var currentMenuModel: null

    // 当前配置文件的根节点属性（version/comment/commentLocal）
    property var rootConfigData: null
    
    // 编辑状态管理
    property string editingFilePath: ""  // 当前正在编辑的文件路径
    property bool isNewFile: false       // 是否是新建文件
    property var editingModelRef: null   // 正在编辑的模型引用
    
    // 右键上下文菜单（移到ApplicationWindow级别以访问所有模型）
    Menu {
        id: contextMenu
        property string filePath: ""
        property var modelRef: null
        
        MenuItem {
            text: qsTr("Refresh List")
            onTriggered: {
                console.log("Refreshing file lists")
                userFileModel.refresh()
                systemFileModel.refresh()
            }
        }
        
        MenuSeparator {}
        
        MenuItem {
            text: qsTr("New File")
            onTriggered: {
                var currentModel = contextMenu.modelRef
                if (currentModel && currentModel.startNewFile) {
                    currentModel.startNewFile()
                    // 设置编辑状态
                    editingFilePath = ""
                    isNewFile = true
                    editingModelRef = currentModel
                }
            }
        }
        
        MenuItem {
            text: qsTr("Rename")
            enabled: contextMenu.filePath !== ""
            onTriggered: {
                console.log("Rename triggered: contextMenu.modelRef =", contextMenu.modelRef, "typeof:", typeof contextMenu.modelRef)
                console.log("Starting inline edit for:", contextMenu.filePath)
                editingFilePath = contextMenu.filePath
                isNewFile = false
                editingModelRef = contextMenu.modelRef
                console.log("Set editingModelRef to:", editingModelRef)
            }
        }
        
        MenuItem {
            text: qsTr("Delete")
            enabled: contextMenu.filePath !== ""
            onTriggered: {
                console.log("Deleting file:", contextMenu.filePath)
                if (contextMenu.modelRef === userFileModel) {
                    userFileModel.deleteFile(contextMenu.filePath)
                } else if (contextMenu.modelRef === systemFileModel) {
                    systemFileModel.deleteFile(contextMenu.filePath)
                }
            }
        }
        
        MenuSeparator {}
        
        MenuItem {
            text: qsTr("Open Containing Folder")
            enabled: contextMenu.filePath !== ""
            onTriggered: {
                console.log("Opening containing folder for:", contextMenu.filePath)
                if (contextMenu.modelRef === userFileModel) {
                    console.log("Calling userFileModel.openContainingFolder")
                    userFileModel.openContainingFolder(contextMenu.filePath)
                } else if (contextMenu.modelRef === systemFileModel) {
                    console.log("Calling systemFileModel.openContainingFolder")
                    systemFileModel.openContainingFolder(contextMenu.filePath)
                } else {
                    console.log("Unknown model, trying both")
                    userFileModel.openContainingFolder(contextMenu.filePath)
                }
            }
        }
        
        MenuItem {
            text: qsTr("Open File")
            enabled: contextMenu.filePath !== ""
            onTriggered: {
                console.log("Opening file:", contextMenu.filePath)
                if (contextMenu.modelRef === userFileModel) {
                    console.log("Calling userFileModel.openFile")
                    userFileModel.openFile(contextMenu.filePath)
                } else if (contextMenu.modelRef === systemFileModel) {
                    console.log("Calling systemFileModel.openFile")
                    systemFileModel.openFile(contextMenu.filePath)
                } else {
                    console.log("Unknown model, trying both")
                    userFileModel.openFile(contextMenu.filePath)
                }
            }
        }
    }
    
    // 当前选中的菜单项
    property var currentItem: null
    // 当前正在编辑的 delegate
    property var editingDelegate: null
    
    // 保存当前选中项的原始值，用于脏检查
    property var originalItemValues: null
    
    // 辅助函数：强制保存 execCommandField 的编辑内容
    function forceSaveExecCommand() {
        if (execCommandField && currentItem && currentMenuModel && originalItemValues) {
            // 检查字段内容是否与模型中的值不同
            var currentText = execCommandField.currentText || ""
            var modelText = currentItem.execCommand || ""
            if (currentText !== modelText) {
                var index = currentMenuModel.getIndex(currentItem.id)
                currentMenuModel.updateItem(index, "execCommand", currentText)
                originalItemValues.execCommand = currentText
                menuManager.saveCurrentModel()
                if (debugLogging) console.log("Force saved execCommand:", currentText)
            }
        }
    }
    
    // 监听 currentItem 变化，保存原始值
    onCurrentItemChanged: {
        if (currentItem) {
            // 深拷贝当前项的值
            originalItemValues = {
                id: currentItem.id || "",
                comment: currentItem.comment || "",
                commentLocal: currentItem.commentLocal || "",
                version: currentItem.version || "",
                name: currentItem.name || "",
                nameLocal: currentItem.nameLocal || "",
                menuTypes: currentItem.menuTypes ? currentItem.menuTypes.slice() : [],
                positionNumber: currentItem.positionNumber || 1,
                supportSuffix: currentItem.supportSuffix ? currentItem.supportSuffix.slice() : [],
                execCommand: currentItem.execCommand || ""
            }
            if (debugLogging) console.log("Saved original values for item:", currentItem.name)
        } else {
            originalItemValues = null
        }
    }
    
    // 辅助函数：更新属性并保存（带脏检查）
    function updateProperty(propertyName, newValue) {
        if (!currentItem || !currentMenuModel || !originalItemValues) {
            return
        }

        // 在更新其他属性之前，强制保存 execCommandField 的编辑内容
        if (propertyName !== "execCommand") {
            forceSaveExecCommand()
        }

        var oldValue = originalItemValues[propertyName]
        var isChanged = false

        // 比较新旧值
        if (Array.isArray(newValue)) {
            isChanged = !Utils.arraysEqual(newValue, oldValue)
        } else {
            // 普通值比较
            isChanged = (newValue !== oldValue)
        }

        if (isChanged) {
            if (debugLogging) console.log("Property", propertyName, "changed from", oldValue, "to", newValue)
            var index = currentMenuModel.getIndex(currentItem.id)
            currentMenuModel.updateItem(index, propertyName, newValue)
            // 更新原始值
            originalItemValues[propertyName] = Array.isArray(newValue) ? newValue.slice() : newValue
            // 保存到文件
            menuManager.saveCurrentModel()
        } else {
            if (debugLogging) console.log("Property", propertyName, "not changed, skipping save")
        }
    }

    // 辅助函数：切换菜单类型
    function toggleMenuType(typeName, isChecked) {
        var types = currentItem.menuTypes ? currentItem.menuTypes.slice() : []
        if (isChecked) {
            if (types.indexOf(typeName) < 0) types.push(typeName)
        } else {
            var idx = types.indexOf(typeName)
            if (idx >= 0) types.splice(idx, 1)
        }
        updateProperty("menuTypes", types)
    }

    // 辅助函数：判断文件是否是系统文件
    function isSystemFile(filePath) {
        return Utils.isSystemFile(filePath)
    }

    // 视图状态保存
    property string savedSelectedItemId: ""
    property var savedExpandedItemIds: []  // 展开的节点ID列表
    
    // 保存列宽
    property real filePanelWidth: 350
    property real menuEditorWidth: 630
    property real propertyPanelWidth: 420
    
    // 连接 MenuManager 信号
    Connections {
        target: menuManager
        function onConfigLoaded(configFile) {
            console.log("Config loaded:", configFile)
            selectedFilePath = configFile
            currentMenuModel = menuManager.getMenuModel(configFile)
            console.log("currentMenuModel:", currentMenuModel)
            console.log("currentMenuModel type:", typeof currentMenuModel)
            if (currentMenuModel) {
                console.log("Menu model loaded, row count:", currentMenuModel.rowCount())
                console.log("TreeView width:", menuTreeView.width, "height:", menuTreeView.height)

                // 提取根节点属性数据
                var allItems = currentMenuModel.getAllItems()
                if (allItems.length > 0) {
                    rootConfigData = allItems[0]
                }
                
                // 展开所有节点
                menuTreeView.expandRecursively(-1, -1)
                
                // 导出 JSON 用于调试
                var jsonString = menuManager.exportToJson(configFile)
                console.log("=== Exported JSON ===")
                console.log(jsonString)
                console.log("=== End of JSON ===")
                
                // 恢复视图状态
                restoreViewState()
            } else {
                console.log("Failed to load menu model")
            }
        }
    }
    
    // 连接当前模型信号，保存视图状态
    Connections {
        target: currentMenuModel
        function onModelAboutToReset() {
            console.log("Model about to reset, saving view state")
            saveViewState()
        }
    }
    
    // 保存视图状态的函数
    function saveViewState() {
        console.log("=== saveViewState called ===")
        console.log("Current item:", currentItem)
        if (currentItem) {
            savedSelectedItemId = currentItem.id || ""
            console.log("Saved selected item ID:", savedSelectedItemId, "name:", currentItem.name)
        } else {
            savedSelectedItemId = ""
            console.log("No current item, savedSelectedItemId set to empty")
        }
        console.log("Saved expanded item IDs:", savedExpandedItemIds)
        menuManager.saveViewState(savedSelectedItemId, savedExpandedItemIds)
        console.log("=== saveViewState completed ===")
    }
    
    // 恢复视图状态的函数
    function restoreViewState() {
        console.log("=== restoreViewState called ===")
        // 从 MenuManager 获取保存的状态
        var selectedId = menuManager.getSelectedItemId()
        var expandedIds = menuManager.getExpandedItemIds()
        
        console.log("Restoring view state - selectedId:", selectedId, "expandedIds:", expandedIds)
        console.log("Current menu model:", currentMenuModel)
        
        // 延迟执行，确保模型已经完全加载
        restoreTimer.expandedIds = expandedIds
        restoreTimer.selectedId = selectedId
        restoreTimer.start()
    }
    
    // 定时器，用于延迟恢复视图状态
    Timer {
        id: restoreTimer
        interval: 300  // 增加延迟时间到 300ms
        property var expandedIds: []
        property string selectedId: ""
        
        onTriggered: {
            console.log("=== restoreTimer triggered ===")
            console.log("Expanded IDs to restore:", expandedIds.length, "items")
            console.log("Selected ID to restore:", selectedId)
            console.log("Current menu model:", currentMenuModel)
            
            // 恢复展开状态
            if (currentMenuModel && expandedIds.length > 0) {
                for (var i = 0; i < expandedIds.length; i++) {
                    var itemId = expandedIds[i]
                    var index = currentMenuModel.getIndex(itemId)
                    console.log("Expanding item:", itemId, "index valid:", index.valid)
                    if (index.valid) {
                        menuTreeView.expand(index.row)
                        console.log("Expanded item:", itemId, "at row:", index.row)
                    }
                }
            }
            
            // 恢复选择状态
            if (currentMenuModel) {
                var found = false
                var allItems = currentMenuModel.getAllItems()
                
                console.log("Searching for selected item:", selectedId, "in", allItems.length, "items")
                
                // 遍历所有项，查找匹配的 ID
                for (var j = 0; j < allItems.length; j++) {
                    var itemData = allItems[j]
                    console.log("Checking item", j, "id:", itemData.id, "name:", itemData.name)
                    if (itemData.id === selectedId) {
                        // 找到匹配的项，设置为当前项
                        currentItem = itemData
                        
                        // 设置 TreeView 的选中状态（使用 selectionModel）
                        var modelIndex = currentMenuModel.getIndex(selectedId)
                        if (modelIndex.valid) {
                            // 使用 select 方法选中行
                            menuTreeView.selectionModel.select(modelIndex, ItemSelectionModel.Select)
                            menuTreeView.selectionModel.setCurrentIndex(modelIndex, ItemSelectionModel.SelectCurrent)
                            console.log("✓ Set TreeView selection to row:", modelIndex.row)
                        }
                        
                        console.log("✓ Restored selected item:", selectedId, "name:", itemData.name)
                        found = true
                        break
                    }
                }
                
                // 如果找不到之前的选择项，默认选择第一项（跳过根节点）
                if (!found && allItems.length > 1) {
                    currentItem = allItems[1]  // 跳过根节点，选择第一个子节点
                    
                    // 设置 TreeView 的选中状态
                    var firstIndex = currentMenuModel.getIndex(allItems[1].id)
                    if (firstIndex.valid) {
                        menuTreeView.selectionModel.select(firstIndex, ItemSelectionModel.Select)
                        menuTreeView.selectionModel.setCurrentIndex(firstIndex, ItemSelectionModel.SelectCurrent)
                        console.log("✓ Set TreeView selection to first item row:", firstIndex.row)
                    }
                    
                    console.log("✗ Selected first item as default:", currentItem.name, "id:", currentItem.id)
                } else if (!found) {
                    console.log("✗ No items found to select")
                }
            } else {
                console.log("✗ No current menu model")
            }
            console.log("=== restoreTimer completed ===")
        }
    }
    
    // 文件类型选择弹窗
    Components.DFileTypeSelectorDialog {
        id: fileTypeSelectorDialog
        
        allFileTypes: fileTypeManager.allFileTypes
        categories: fileTypeManager.categories
        
        onOpened: {
            // 初始化已选择的后缀列表
            if (currentItem && currentItem.supportSuffix) {
                selectedSuffixes = currentItem.supportSuffix.slice()  // 复制数组
            } else {
                selectedSuffixes = []
            }
            console.log("Dialog opened, current suffixes:", selectedSuffixes)
        }
        
        onAccepted: {
            // 确认选择，更新后缀列表
            if (currentItem && currentMenuModel) {
                // 使用 updateProperty 进行脏检查和保存
                updateProperty("supportSuffix", selectedSuffixes.slice())
                // 不再显式设置 suffixTextArea.text，让绑定自动更新
                console.log("Suffixes updated:", selectedSuffixes.join(":"))
            }
        }
        
        onSelectionChanged: function(suffixes) {
            console.log("Selection changed:", suffixes.join(":"))
        }
    }
    
    // ==================== 全局提示组件 ====================
    
    /**
     * 全局提示组件实例
     * 用于显示信息或警告提示
     * 
     * 使用方法：
     * 1. 通过 showInfo() 或 showWarning() 函数显示提示
     * 2. 提示会在4秒后自动消失（除非设置 persistent: true）
     * 3. 用户可以点击关闭按钮立即关闭提示
     * 
     * 示例代码：
     * showInfo("操作成功")
     * showWarning("操作失败，请重试")
     * 
     * 或者直接创建组件实例：
     * var prompt = globalPromptComponent.createObject(root, {
     *     message: "自定义提示消息",
     *     type: "info",
     *     persistent: false
     * })
     */
    Component {
        id: globalPromptComponent
        
        Components.DGlobalPrompt {
            // 组件属性在使用时动态设置
        }
    }
    
    /**
     * 显示信息提示
     * @param message 提示消息文本
     * @param autoCloseInterval 自动关闭时间（毫秒），可选，默认4000ms
     */
    function showInfo(message, autoCloseInterval) {
        var properties = {
            message: message,
            type: "info",
            persistent: false
        }
        if (autoCloseInterval !== undefined) {
            properties.autoCloseInterval = autoCloseInterval
        }
        var prompt = globalPromptComponent.createObject(root.contentItem, properties)
        if (!prompt) {
            console.error("Failed to create DGlobalPrompt")
        } else {
            console.log("DGlobalPrompt created successfully:", message)
        }
    }
    
    /**
     * 显示警告提示
     * @param message 提示消息文本
     * @param autoCloseInterval 自动关闭时间（毫秒），可选，默认4000ms
     * @param persistent 是否常驻显示（不自动关闭），可选，默认false
     */
    function showWarning(message, autoCloseInterval, persistent) {
        var properties = {
            message: message,
            type: "warning",
            persistent: persistent !== undefined ? persistent : false
        }
        if (autoCloseInterval !== undefined && !properties.persistent) {
            properties.autoCloseInterval = autoCloseInterval
        }
        var prompt = globalPromptComponent.createObject(root.contentItem, properties)
        if (!prompt) {
            console.error("Failed to create DGlobalPrompt")
        }
    }
    
    /**
     * 显示常驻提示（不自动消失）
     * @param message 提示消息文本
     * @param type 提示类型（"info"或"warning"），默认为"info"
     */
    function showPersistentPrompt(message, type) {
        var prompt = globalPromptComponent.createObject(root.contentItem, {
            message: message,
            type: type || "info",
            persistent: true
        })
        if (!prompt) {
            console.error("Failed to create DGlobalPrompt")
        }
    }
}

