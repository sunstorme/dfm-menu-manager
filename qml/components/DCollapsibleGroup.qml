// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import "qrc:/qml/styles" as Styles

/**
 * DCollapsibleGroup - 可折叠分组组件
 *
 * 带标题栏的折叠/展开容器，内容区域支持动画过渡。
 *
 * 用法：
 *   DCollapsibleGroup {
 *       title: qsTr("User Configuration")
 *       expanded: true
 *
 *       ListView { ... }   // 直接放子元素即可
 *   }
 */
Rectangle {
    id: root

    /** 标题文本 */
    property string title: ""

    /** 是否展开 */
    property bool expanded: true

    /** 直接在标签内放子元素 */
    default property alias content: contentItem.data

    // --- 外观 ---
    color: Styles.Style.backgroundColor
    border.color: Styles.Style.borderColor
    border.width: 1
    radius: Styles.Style.borderRadius

    implicitHeight: headerBar.height + (expanded ? contentArea.height + Styles.Style.padding : 0)

    Behavior on implicitHeight {
        NumberAnimation {
            duration: Styles.Style.animationDuration
            easing.type: Easing.InOutQuad
        }
    }

    // --- 标题栏 ---
    Rectangle {
        id: headerBar
        width: parent.width
        height: Styles.Style.itemHeight
        color: Styles.Style.hoverColor
        radius: Styles.Style.borderRadius

        Text {
            anchors.left: parent.left
            anchors.leftMargin: Styles.Style.padding
            anchors.verticalCenter: parent.verticalCenter
            text: root.title
            font: Styles.Style.h2Font
            color: Styles.Style.textColor
        }

        Text {
            anchors.right: parent.right
            anchors.rightMargin: Styles.Style.padding
            anchors.verticalCenter: parent.verticalCenter
            text: root.expanded ? "▼" : "▶"
            font: Styles.Style.tagFont
            color: Styles.Style.secondaryTextColor
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.expanded = !root.expanded
        }
    }

    // --- 内容区域 ---
    Item {
        id: contentArea
        anchors.top: headerBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: Styles.Style.padding
        height: root.expanded ? contentItem.height + Styles.Style.padding: 0
        clip: true
        visible: root.expanded

        Item {
            id: contentItem
            width: parent.width
            height: childrenRect.height
        }
    }
}
