#include "config_parser.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QSet>

ConfigParser::ConfigData ConfigParser::parseFile(const QString &filePath) {
    ConfigData data;
    QFile file(filePath);
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开配置文件:" << filePath;
        return data;
    }
    
    MenuActionItem *currentAction = nullptr;
    QString currentGroup;
    
    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();
        
        // 跳过空行和注释
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }
        
        // 解析组头 [Menu Action xxx]
        if (line.startsWith('[') && line.endsWith(']')) {
            currentGroup = line.mid(1, line.length() - 2);
            
            if (currentGroup.startsWith("Menu Action ")) {
                QString actionId = currentGroup.mid(12);
                currentAction = new MenuActionItem();
                currentAction->id = actionId;
                currentAction->isRoot = false;
                currentAction->configFile = filePath;
                data.actions.append(*currentAction);
                data.actionMap[actionId] = currentAction;
            } else if (currentGroup == "Menu Entry") {
                currentAction = new MenuActionItem();
                currentAction->isRoot = true;
                currentAction->id = "root";
                currentAction->configFile = filePath;
                data.rootActionId = "root";
                data.actions.append(*currentAction);
                data.actionMap["root"] = currentAction;
            }
            continue;
        }
        
        // 解析键值对
        QString key, value;
        if (parseLine(line, key, value)) {
            if (!currentAction) {
                continue;
            }
            
            // 处理各种字段
            if (key == "Name") {
                currentAction->name = value;
            } else if (key == "Name[zh_CN]") {
                currentAction->nameLocal = value;
            } else if (key == "Comment") {
                if (currentAction->isRoot) {
                    data.comment = value;
                } else {
                    currentAction->comment = value;
                }
            } else if (key == "Comment[zh_CN]") {
                if (currentAction->isRoot) {
                    data.commentLocal = value;
                } else {
                    currentAction->commentLocal = value;
                }
            } else if (key == "Version") {
                data.version = value;
            } else if (key == "Actions") {
                currentAction->childActions = parseActions(value);
            } else if (key == "X-DFM-MenuTypes") {
                currentAction->menuTypes = parseList(value, ":");
            } else if (key == "X-DFM-SupportSuffix") {
                currentAction->supportSuffix = parseList(value, ":");
            } else if (key.startsWith("PosNum")) {
                currentAction->positionNumber = value.toInt();
            } else if (key == "Exec") {
                currentAction->execCommand = value;
            } else if (key == "Separator") {
                if (value == "Top") {
                    currentAction->separatorTop = true;
                } else if (value == "Bottom") {
                    currentAction->separatorBottom = true;
                }
            }
        }
    }
    
    // 构建树形结构
    buildTreeStructure(data);
    
    file.close();
    return data;
}

bool ConfigParser::parseLine(const QString &line, QString &key, QString &value) {
    int equalPos = line.indexOf('=');
    if (equalPos <= 0) {
        return false;
    }
    
    key = line.left(equalPos).trimmed();
    value = line.mid(equalPos + 1).trimmed();
    return !key.isEmpty();
}

QStringList ConfigParser::parseActions(const QString &actionsStr) {
    return parseList(actionsStr, ":");
}

QStringList ConfigParser::parseList(const QString &listStr, const QString &separator) {
    QStringList result;
    QStringList items = listStr.split(separator);
    
    for (const QString &item : items) {
        QString trimmed = item.trimmed();
        if (!trimmed.isEmpty()) {
            result.append(trimmed);
        }
    }
    
    return result;
}

bool ConfigParser::validate(const ConfigData &data) {
    return getValidationErrors(data).isEmpty();
}

QStringList ConfigParser::getValidationErrors(const ConfigData &data) {
    QStringList errors;
    
    // 检查版本号
    if (data.version.isEmpty()) {
        errors << "缺少版本号";
    }
    
    // 检查根菜单
    if (!data.actionMap.contains("root")) {
        errors << "缺少根菜单项";
    }
    
    // 检查菜单项ID唯一性
    QSet<QString> ids;
    for (const auto &action : data.actions) {
        if (ids.contains(action.id)) {
            errors << QString("重复的菜单项ID: %1").arg(action.id);
        }
        ids.insert(action.id);
    }
    
    // 检查子菜单引用
    for (const auto &action : data.actions) {
        for (const QString &childId : action.childActions) {
            if (!data.actionMap.contains(childId)) {
                errors << QString("引用的子菜单不存在: %1").arg(childId);
            }
        }
    }
    
    // 检查菜单层级
    for (const auto &action : data.actions) {
        if (action.level > 3) {
            errors << QString("菜单项 %1 超过最大层级(3)").arg(action.name);
        }
    }
    
    // 检查必需字段
    for (const auto &action : data.actions) {
        if (action.name.isEmpty()) {
            errors << QString("菜单项缺少名称: %1").arg(action.id);
        }
    }
    
    return errors;
}

void ConfigParser::buildTreeStructure(ConfigData &data) {
    for (auto &action : data.actions) {
        calculateLevel(action, data);
    }
}

void ConfigParser::calculateLevel(MenuActionItem &action, ConfigData &data) {
    if (action.isRoot) {
        action.level = 0;
        return;
    }
    
    // 查找父级
    for (auto &parent : data.actions) {
        if (parent.childActions.contains(action.id)) {
            action.level = parent.level + 1;
            if (action.level > 3) {
                action.level = 3;
            }
            return;
        }
    }
    
    action.level = 1;
}
