// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Controls
import "qrc:/qml/styles" as Styles

/**
 * 带标签的属性编辑字段组件
 * 封装了 Text 标签 + DTextField 编辑框的通用模式，
 * 支持 Enter/Return 确认和 EditingFinished 自动保存。
 *
 * 使用示例：
 * DPropertyField {
 *     labelText: qsTr("Description")
 *     propertyName: "comment"
 *     fieldValue: currentItem ? currentItem.comment || "" : ""
 *     onValueEdited: function(name, value) {
 *         updateProperty(name, value)
 *     }
 * }
 */
Column {
    id: root

    property string labelText: ""
    property string fieldValue: ""
    property string propertyName: ""

    signal valueEdited(string name, var value)

    spacing: 5

    Text {
        text: root.labelText
        font: Styles.Style.h3Font
        color: Styles.Style.secondaryTextColor
    }

    DTextField {
        width: root.width
        height: Styles.Style.itemHeight
        text: root.fieldValue

        onEditingFinished: {
            root.valueEdited(root.propertyName, text)
        }

        Keys.onPressed: function(event) {
            if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                root.valueEdited(root.propertyName, text)
                event.accepted = true
            }
        }
    }
}
