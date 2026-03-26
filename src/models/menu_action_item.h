// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MENUACTIONITEM_H
#define MENUACTIONITEM_H

#include <QString>
#include <QStringList>
#include <QMap>

/**
 * @brief 菜单项数据结构
 * 
 * 表示DFM右键菜单中的一个菜单项,支持最多3级嵌套
 */
class MenuActionItem {
public:
    MenuActionItem();
    
    // 基本信息
    QString id;                      // 唯一标识符
    QString name;                    // 菜单名称
    QString nameLocal;               // 本地化名称(中文)
    QString comment;                 // 描述
    QString commentLocal;            // 本地化描述
    
    // 菜单配置
    QStringList menuTypes;           // 菜单类型列表 (SingleFile, MultiFiles等)
    QStringList supportSuffix;       // 支持的文件后缀
    int positionNumber;              // 位置编号
    QMap<QString, int> positionByType; // 按类型的位置
    bool separatorTop;               // 顶部分隔符
    bool separatorBottom;            // 底部分隔符
    
    // 动作配置
    QString execCommand;             // 执行命令
    QStringList childActions;        // 子菜单ID列表
    
    // 元数据
    bool isRoot;                     // 是否为根菜单项
    int level;                       // 菜单层级 (0-3, 0为根)
    QString configFile;              // 所属配置文件
    bool isSystem;                   // 是否为系统配置
    
    // 辅助方法
    bool hasChildren() const { return !childActions.isEmpty(); }
    bool isLeaf() const { return childActions.isEmpty(); }
    
    // 运算符
    bool operator==(const MenuActionItem &other) const {
        return id == other.id;
    }
};

#endif // MENUACTIONITEM_H
