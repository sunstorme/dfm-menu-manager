// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CONFIGWRITER_H
#define CONFIGWRITER_H

#include <QString>
#include "../core/config_parser.h"

/**
 * @brief 配置文件写入器
 * 
 * 负责将配置数据写入DFM格式的配置文件
 */
class ConfigWriter {
public:
    // 写入方法
    static bool writeToFile(const QString &filePath, 
                           const ConfigParser::ConfigData &data);
    static bool backupFile(const QString &filePath);
    
    // 格式化方法
    static QString formatEntry(const MenuActionItem &item);
    static QString formatComment(const QString &text);
    static QString formatList(const QStringList &list, 
                             const QString &separator);
};

#endif // CONFIGWRITER_H
