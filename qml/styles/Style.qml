// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
pragma Singleton
import QtQuick

QtObject {
    // 颜色
    readonly property color primaryColor: "#0081FF"
    readonly property color backgroundColor: "#FFFFFF"
    readonly property color borderColor: "#E5E5E5"
    readonly property color textColor: "#000000"
    readonly property color secondaryTextColor: "#888888"
    readonly property color selectColor: "#E8F3FF"
    readonly property color hoverColor: "#F5F5F5"
    readonly property color systemTagColor: "#FF6A00"
    readonly property color errorColor: "#FF5722"
    readonly property color successColor: "#00C853"
    
    // 字体
    readonly property font titleFont: Qt.font({
        family: "Noto Sans CJK SC",
        pixelSize: 16,
        bold: true
    })
    
    readonly property font itemFont: Qt.font({
        family: "Noto Sans CJK SC",
        pixelSize: 14
    })
    
    readonly property font tagFont: Qt.font({
        family: "Noto Sans CJK SC",
        pixelSize: 12
    })
    
    readonly property font smallFont: Qt.font({
        family: "Noto Sans CJK SC",
        pixelSize: 12
    })
    
    // 尺寸
    readonly property int spacing: 10
    readonly property int padding: 15
    readonly property int borderRadius: 4
    readonly property int itemHeight: 40
    readonly property int toolbarHeight: 50
    
    // 动画
    readonly property int animationDuration: 200
}
