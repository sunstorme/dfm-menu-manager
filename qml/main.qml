import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import DFMMenu 1.0
import "qrc:/qml/styles" as Styles

ApplicationWindow {
    id: root
    visible: true
    width: 1400
    height: 900
    minimumWidth: 1000
    minimumHeight: 600
    title: qsTr("DFM 右键菜单管理器")
    
    // 窗口状态管理
    Component.onCompleted: {
        // WindowManager.restoreState(root)
    }
    
    Component.onDestruction: {
        WindowManager.saveState(root)
    }
    
    // 主布局 - 使用SplitView实现可拖动分隔器
    SplitView {
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
                TextField {
                    id: searchBox
                    width: parent.width
                    height: Styles.Style.itemHeight
                    placeholderText: qsTr("搜索配置文件...")
                    font.pixelSize: 14
                    
                    background: Rectangle {
                        color: Styles.Style.backgroundColor
                        border.color: Styles.Style.borderColor
                        border.width: 1
                        radius: Styles.Style.borderRadius
                    }
                }
                
                ScrollView {
                    width: parent.width
                    height: parent.height - searchBox.height - Styles.Style.spacing
                    
                    ColumnLayout {
                        width: parent.width
                        spacing: Styles.Style.spacing
                        
                        // 用户配置文件区(可折叠)
                        Rectangle {
                            id: userGroupBox
                            Layout.fillWidth: true
                            color: Styles.Style.backgroundColor
                            border.color: Styles.Style.borderColor
                            border.width: 1
                            radius: Styles.Style.borderRadius
                            
                            implicitHeight: userHeader.height + (userExpanded ? userContent.height + Styles.Style.padding : 0)
                            
                            Behavior on implicitHeight {
                                NumberAnimation {
                                    duration: Styles.Style.animationDuration
                                    easing.type: Easing.InOutQuad
                                }
                            }
                            
                            // 标题栏
                            Rectangle {
                                id: userHeader
                                width: parent.width
                                height: Styles.Style.itemHeight
                                color: Styles.Style.hoverColor
                                radius: Styles.Style.borderRadius
                                
                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: Styles.Style.padding
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("用户配置")
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: Styles.Style.textColor
                                }
                                
                                Text {
                                    anchors.right: parent.right
                                    anchors.rightMargin: Styles.Style.padding
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: userExpanded ? "▼" : "▶"
                                    font.pixelSize: 12
                                    color: Styles.Style.secondaryTextColor
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        userExpanded = !userExpanded
                                    }
                                }
                            }
                            
                            // 内容区域
                            Item {
                                id: userContent
                                anchors.top: userHeader.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: Styles.Style.padding
                                height: userExpanded ? userList.height : 0
                                clip: true
                                
                                ListView {
                                    id: userList
                                    anchors.fill: parent
                                    model: MenuFileModel {
                                        id: userFileModel
                                        showSystemOnly: false
                                    }
                                    delegate: fileDelegate
                                    clip: true
                                }
                            }
                        }
                        
                        // 系统配置文件区(可折叠)
                        Rectangle {
                            id: systemGroupBox
                            Layout.fillWidth: true
                            color: Styles.Style.backgroundColor
                            border.color: Styles.Style.borderColor
                            border.width: 1
                            radius: Styles.Style.borderRadius
                            
                            implicitHeight: systemHeader.height + (systemExpanded ? systemContent.height + Styles.Style.padding : 0)
                            
                            Behavior on implicitHeight {
                                NumberAnimation {
                                    duration: Styles.Style.animationDuration
                                    easing.type: Easing.InOutQuad
                                }
                            }
                            
                            // 标题栏
                            Rectangle {
                                id: systemHeader
                                width: parent.width
                                height: Styles.Style.itemHeight
                                color: Styles.Style.hoverColor
                                radius: Styles.Style.borderRadius
                                
                                Text {
                                    anchors.left: parent.left
                                    anchors.leftMargin: Styles.Style.padding
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: qsTr("系统配置")
                                    font.pixelSize: 14
                                    font.bold: true
                                    color: Styles.Style.textColor
                                }
                                
                                Text {
                                    anchors.right: parent.right
                                    anchors.rightMargin: Styles.Style.padding
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: systemExpanded ? "▼" : "▶"
                                    font.pixelSize: 12
                                    color: Styles.Style.secondaryTextColor
                                }
                                
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        systemExpanded = !systemExpanded
                                    }
                                }
                            }
                            
                            // 内容区域
                            Item {
                                id: systemContent
                                anchors.top: systemHeader.bottom
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.margins: Styles.Style.padding
                                height: systemExpanded ? systemList.height : 0
                                clip: true
                                
                                ListView {
                                    id: systemList
                                    anchors.fill: parent
                                    model: MenuFileModel {
                                        id: systemFileModel
                                        showSystemOnly: true
                                    }
                                    delegate: fileDelegate
                                    clip: true
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
        
        // 分隔器
        Rectangle {
            SplitView.minimumWidth: 1
            SplitView.preferredWidth: 1
            SplitView.maximumWidth: 1
            color: Styles.Style.borderColor
        }
        
        // 中间面板 - 菜单编辑区
        Rectangle {
            id: menuEditor
            SplitView.minimumWidth: 400
            SplitView.preferredWidth: menuEditorWidth
            color: Styles.Style.backgroundColor
            
            Column {
                anchors.fill: parent
                spacing: 0
                
                // 工具栏
                Rectangle {
                    width: parent.width
                    height: Styles.Style.toolbarHeight
                    color: Styles.Style.backgroundColor
                    
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("菜单结构编辑")
                        font.pixelSize: 16
                        font.bold: true
                        color: Styles.Style.textColor
                    }
                }
                
                // 菜单树视图
                Rectangle {
                    width: parent.width
                    height: parent.height - Styles.Style.toolbarHeight
                    color: Styles.Style.backgroundColor
                    
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("菜单树视图")
                        font.pixelSize: 14
                        color: Styles.Style.secondaryTextColor
                    }
                }
            }
            
            onWidthChanged: {
                menuEditorWidth = width
            }
        }
        
        // 分隔器
        Rectangle {
            SplitView.minimumWidth: 1
            SplitView.preferredWidth: 1
            SplitView.maximumWidth: 1
            color: Styles.Style.borderColor
        }
        
        // 右侧面板 - 属性编辑区
        Rectangle {
            id: propertyPanel
            SplitView.minimumWidth: 300
            SplitView.preferredWidth: propertyPanelWidth
            color: Styles.Style.backgroundColor
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: Styles.Style.padding
                
                Column {
                    width: parent.width
                    spacing: Styles.Style.spacing * 2
                    
                    Text {
                        text: qsTr("属性编辑")
                        font.pixelSize: 16
                        font.bold: true
                        color: Styles.Style.textColor
                    }
                    
                    // 菜单名称
                    Column {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            text: qsTr("菜单名称")
                            font.pixelSize: 12
                            color: Styles.Style.secondaryTextColor
                        }
                        
                        TextField {
                            width: parent.width
                            height: Styles.Style.itemHeight
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: Styles.Style.backgroundColor
                                border.color: Styles.Style.borderColor
                                border.width: 1
                                radius: Styles.Style.borderRadius
                            }
                        }
                    }
                    
                    // 菜单名称(中文)
                    Column {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            text: qsTr("菜单名称(中文)")
                            font.pixelSize: 12
                            color: Styles.Style.secondaryTextColor
                        }
                        
                        TextField {
                            width: parent.width
                            height: Styles.Style.itemHeight
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: Styles.Style.backgroundColor
                                border.color: Styles.Style.borderColor
                                border.width: 1
                                radius: Styles.Style.borderRadius
                            }
                        }
                    }
                    
                    // 可执行命令
                    Column {
                        width: parent.width
                        spacing: 5
                        
                        Text {
                            text: qsTr("可执行命令")
                            font.pixelSize: 12
                            color: Styles.Style.secondaryTextColor
                        }
                        
                        TextField {
                            width: parent.width
                            height: Styles.Style.itemHeight
                            font.pixelSize: 14
                            
                            background: Rectangle {
                                color: Styles.Style.backgroundColor
                                border.color: Styles.Style.borderColor
                                border.width: 1
                                radius: Styles.Style.borderRadius
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
    
    // 文件列表委托
    Component {
        id: fileDelegate
        
        Rectangle {
            width: ListView.view ? ListView.view.width : 100
            height: Styles.Style.itemHeight
            color: mouseArea.containsMouse ? Styles.Style.hoverColor : 
                   mouseArea.pressed ? Styles.Style.selectColor : "transparent"
            
            Text {
                anchors.left: parent.left
                anchors.leftMargin: Styles.Style.padding
                anchors.verticalCenter: parent.verticalCenter
                text: model.fileName || ""
                font.pixelSize: 14
                color: Styles.Style.textColor
            }
            
            Text {
                anchors.right: parent.right
                anchors.rightMargin: Styles.Style.padding
                anchors.verticalCenter: parent.verticalCenter
                text: model.isSystem ? qsTr("系统") : ""
                font.pixelSize: 12
                color: Styles.Style.systemTagColor
                visible: model.isSystem
            }
            
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    menuManager.setCurrentConfig(model.filePath || "")
                }
            }
        }
    }
    
    // 折叠状态
    property bool userExpanded: true
    property bool systemExpanded: false
    
    // 保存列宽
    property real filePanelWidth: 350
    property real menuEditorWidth: 630
    property real propertyPanelWidth: 420
}
