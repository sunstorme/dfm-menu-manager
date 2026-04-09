// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import "qrc:/qml/styles" as Styles

/**
 * 带标签的多行属性编辑字段组件
 * 封装了 Text 标签 + ScrollView(DTextArea) 的通用模式，
 * 失去焦点时通过 signal 通知外部保存。
 *
 * 使用示例（带标签）：
 * DMultiLinePropertyField {
 *     labelText: qsTr("Executable Command")
 *     fieldValue: currentItem ? currentItem.execCommand || "" : ""
 *     onValueEdited: function(value) { updateProperty("execCommand", value) }
 * }
 *
 * 使用示例（无标签，配合外部布局）：
 * Row {
 *     DMultiLinePropertyField {
 *         labelText: ""
 *         width: parent.width - button.width
 *         fieldHeight: parent.height
 *         fieldValue: ...
 *         onValueEdited: function(value) { ... }
 *     }
 *     Button { ... }
 * }
 */
Column {
    id: root

    property string labelText: ""
    property string fieldValue: ""
    property string placeholderText: ""
    property real fieldHeight: Styles.Style.itemHeight * 3

    // 暴露内部编辑框状态，供外部访问（如强制保存）
    readonly property alias currentText: textArea.text
    readonly property alias hasFocus: textArea.focus

    signal valueEdited(var value)

    spacing: labelText !== "" ? 5 : 0

    Text {
        visible: root.labelText !== ""
        text: root.labelText
        font: Styles.Style.h3Font
        color: Styles.Style.secondaryTextColor
    }

    ScrollView {
        width: root.width
        height: root.fieldHeight
        clip: true

        DTextArea {
            id: textArea
            width: parent.width
            height: parent.height
            wrapMode: TextArea.Wrap
            text: root.fieldValue
            placeholderText: root.placeholderText

            onEditingFinished: {
                root.valueEdited(textArea.text)
            }
        }
    }
}
