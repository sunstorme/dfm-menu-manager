// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <QString>
#include <QList>
#include <QMap>
#include "../models/menu_action_item.h"

/**
 * @brief 配置文件解析器
 * 
 * 负责解析DFM右键菜单的配置文件(.conf格式)
 */
class ConfigParser {
public:
    /**
     * @brief 配置数据结构
     */
    struct ConfigData {
        QString version;                    // 版本号
        QString comment;                    // 描述
        QString commentLocal;               // 本地化描述
        QList<MenuActionItem> actions;      // 所有菜单项
        QString rootActionId;               // 根菜单ID
        QMap<QString, MenuActionItem*> actionMap;  // ID到项的映射
        
        bool isValid() const {
            return !version.isEmpty() && !rootActionId.isEmpty();
        }
    };
    
    // 解析方法
    static ConfigData parseFile(const QString &filePath);
    static bool parseLine(const QString &line, QString &key, QString &value);
    static QStringList parseActions(const QString &actionsStr);
    static QStringList parseList(const QString &listStr, const QString &separator);
    
    // 验证方法
    static bool validate(const ConfigData &data);
    static QStringList getValidationErrors(const ConfigData &data);
    
private:
    static void buildTreeStructure(ConfigData &data);
    static void calculateLevel(MenuActionItem &action, ConfigData &data);
};

#endif // CONFIGPARSER_H
