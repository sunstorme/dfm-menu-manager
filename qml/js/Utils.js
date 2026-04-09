// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
.pragma library

/**
 * 判断文件是否是系统文件
 * @param filePath 文件路径
 * @return 如果是系统文件返回 true，否则返回 false
 */
function isSystemFile(filePath) {
    return filePath.indexOf("/usr/share/") !== -1
}

/**
 * 比较两个数组是否相等
 * @param a 第一个数组
 * @param b 第二个数组
 * @return 如果数组内容相同返回 true，否则返回 false
 */
function arraysEqual(a, b) {
    if (!a || !b) return false
    if (a.length !== b.length) return false
    for (var i = 0; i < a.length; i++) {
        if (a[i] !== b[i]) return false
    }
    return true
}

/**
 * 解析后缀字符串为数组
 * @param text 后缀文本（如 "mp4:avi:mkv"）
 * @param separator 分隔符，默认为 ":"
 * @return 过滤空值后的后缀数组
 */
function parseSuffixes(text, separator) {
    if (!text) return []
    return text.split(separator || ":").filter(function(s) { return s.trim() !== "" })
}
