// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "qrc:/qml/styles" as Styles


Rectangle {
    id: root

    property string type: "info"
    property string message: ""
    property bool persistent: false
    property int autoCloseInterval: 4000
    property int maxWidth: 600
    property int padding: 12
    property int iconTextSpacing: 12
    property int textButtonSpacing: 12
    property int iconSize: 24
    property int closeButtonSize: 24
    property int borderRadius: 8
    property int shadowOffset: 2
    property int shadowBlur: 8
    property color shadowColor: "#40000000"
    property int fadeOutDuration: 300
    property bool isClosing: false

    color: type === "warning" ? "#FFF3E0" : "#E3F2FD"
    
    border.color: type === "warning" ? "#FFB74D" : "#64B5F6"
    border.width: 1
    
    radius: borderRadius
 
    width: Math.min(contentRow.implicitWidth + padding * 2, maxWidth)
    height: Math.max(contentRow.implicitHeight + padding * 2, closeButtonSize + padding * 2)
    property var targetParent: parent || null
    
    // 计算位置：水平居中，底部边距40px
    x: {
        if (targetParent) {
            return (targetParent.width - width) / 2
        }
        return 100  // 默认值
    }
    
    y: {
        if (targetParent) {
            // 确保组件在窗口内，底部距离窗口底部40px
            var yPos = targetParent.height - height - 40
            // 如果计算出的y值小于0，则设置为0，防止组件显示在窗口外
            return Math.max(0, yPos)
        }
        return 100  // 默认值
    }
    
    // 确保层级置顶
    z: 999999

    // 淡入动画
    PropertyAnimation {
        id: fadeInAnimation
        target: root
        property: "opacity"
        from: 0
        to: 1
        duration: 200
        easing.type: Easing.InOutQuad
        onStarted: {
            console.log("fadeInAnimation started")
        }
        onFinished: {
            console.log("fadeInAnimation finished, opacity:", root.opacity)
        }
    }
    
    // 淡出动画
    PropertyAnimation {
        id: fadeOutAnimation
        target: root
        property: "opacity"
        from: 1
        to: 0
        duration: root.fadeOutDuration
        easing.type: Easing.InOutQuad
        onFinished: {
            root.destroy()
        }
    }

    // 自动关闭定时器,在非persistent模式下，倒计时结束后自动淡出并销毁组件
    Timer {
        id: autoCloseTimer
        interval: root.autoCloseInterval
        repeat: false
        onTriggered: {
            if (!root.persistent && !root.isClosing) {
                root.close()
            }
        }
    }
    
    Row {
        id: contentRow
        anchors.centerIn: parent
        spacing: 0
        
        // 左侧：状态图标
        Image {
            id: typeIcon
            source: root.type === "warning" ? "qrc:/asset/warning.svg" : "qrc:/asset/info.svg"
            sourceSize.width: root.iconSize
            sourceSize.height: root.iconSize
            width: root.iconSize
            height: root.iconSize
            fillMode: Image.PreserveAspectFit
            asynchronous: true
            cache: true
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // 图标与文本之间的间距
        Item {
            width: root.iconTextSpacing
            height: 1
        }
        
        // 中间：文本区域
        Text {
            id: messageText
            text: root.message
            font: Styles.Style.bodyFont
            color: Styles.Style.textColor
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            // 限制最大宽度，确保文本能够自动换行
            width: Math.min(implicitWidth, root.maxWidth - root.iconSize - root.closeButtonSize - 
                           root.iconTextSpacing - root.textButtonSpacing - root.padding * 4)
            anchors.verticalCenter: parent.verticalCenter
        }
        
        // 文本与关闭按钮之间的间距
        Item {
            width: root.textButtonSpacing
            height: 1
        }
        
        // 右侧：圆形关闭按钮
        Rectangle {
            id: closeButton
            width: root.closeButtonSize
            height: root.closeButtonSize
            radius: width / 2
            color: closeButtonMouseArea.containsMouse ? 
                   (root.type === "warning" ? "#FFE0B2" : "#BBDEFB") : 
                   (root.type === "warning" ? "#FFF3E0" : "#E3F2FD")
            border.color: root.type === "warning" ? "#FFB74D" : "#64B5F6"
            border.width: 1
            anchors.verticalCenter: parent.verticalCenter
            
            // 关闭图标（X）
            Text {
                anchors.centerIn: parent
                text: "✕"
                color: Styles.Style.textColor
                font.pixelSize: 16
            }
            
            // 鼠标区域
            MouseArea {
                id: closeButtonMouseArea
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.PointingHandCursor
                onClicked: {
                    root.close()
                }
            }
            
            // 按下效果
            Behavior on color {
                ColorAnimation {
                    duration: Styles.Style.animationDuration
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }
    
    function show() {
        console.log("DGlobalPrompt.show() called")
        console.log("targetParent:", targetParent)
        console.log("parent:", parent)
        console.log("width:", width, "height:", height)
        console.log("x:", x, "y:", y)
        root.visible = true
        root.isClosing = false
        root.opacity = 0
        fadeInAnimation.start()
        
        if (!root.persistent) {
            autoCloseTimer.restart()
        }
    }

    function close() {
        if (root.isClosing) {
            return
        }
        
        root.isClosing = true
        autoCloseTimer.stop()
        fadeOutAnimation.start()
    }
    
    function showMessage(msg, msgType) {
        root.message = msg
        if (msgType !== undefined) {
            root.type = msgType
        }
        root.show()
    }

    Component.onCompleted: {
        // 组件创建完成后自动显示
        root.show()
    }
}
