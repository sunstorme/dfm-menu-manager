// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "config_parser.h"
#include "../utils/logger.h"
#include "../utils/constants.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSet>

ConfigParser::ConfigData ConfigParser::parseFile(const QString &filePath) {
    ConfigData data;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_WARNING(QString("无法打开配置文件: %1").arg(filePath));
        return data;
    }

    QSharedPointer<MenuActionItem> currentAction;
    QString currentGroup;

    while (!file.atEnd()) {
        QString line = QString::fromUtf8(file.readLine()).trimmed();

        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        if (line.startsWith('[') && line.endsWith(']')) {
            currentGroup = line.mid(Constants::BRACKET_OFFSET, line.length() - 2);

            if (currentGroup.startsWith(Constants::Config::MENU_ACTION_PREFIX)) {
                QString actionId = currentGroup.mid(Constants::MENU_ACTION_PREFIX_LENGTH);
                currentAction = QSharedPointer<MenuActionItem>(new MenuActionItem());
                currentAction->id = actionId;
                currentAction->isRoot = false;
                currentAction->configFile = filePath;
                data.actions.append(*currentAction);
                data.actionMap[actionId] = currentAction;
            } else if (currentGroup == Constants::Config::MENU_ENTRY_GROUP) {
                currentAction = QSharedPointer<MenuActionItem>(new MenuActionItem());
                currentAction->isRoot = true;
                currentAction->id = Constants::Defaults::ROOT_ACTION_ID;
                currentAction->configFile = filePath;
                data.rootActionId = Constants::Defaults::ROOT_ACTION_ID;
                data.actions.append(*currentAction);
                data.actionMap[Constants::Defaults::ROOT_ACTION_ID] = currentAction;
            }
            continue;
        }

        QString key, value;
        if (parseLine(line, key, value)) {
            if (!currentAction) {
                continue;
            }

            if (key == Constants::Config::KEY_NAME) {
                currentAction->name = value;
            } else if (key == Constants::Config::KEY_NAME_LOCAL) {
                currentAction->nameLocal = value;
            } else if (key == Constants::Config::KEY_COMMENT) {
                if (currentAction->isRoot) {
                    data.comment = value;
                } else {
                    currentAction->comment = value;
                }
            } else if (key == Constants::Config::KEY_COMMENT_LOCAL) {
                if (currentAction->isRoot) {
                    data.commentLocal = value;
                } else {
                    currentAction->commentLocal = value;
                }
            } else if (key == Constants::Config::KEY_VERSION) {
                data.version = value;
            } else if (key == Constants::Config::KEY_ACTIONS) {
                currentAction->childActions = parseActions(value);
            } else if (key == Constants::Config::KEY_MENU_TYPES) {
                currentAction->menuTypes = parseList(value, ":");
            } else if (key == Constants::Config::KEY_SUPPORT_SUFFIX) {
                currentAction->supportSuffix = parseList(value, ":");
            } else if (key == Constants::Config::KEY_POS_NUM) {
                currentAction->positionNumber = value.toInt();
            } else if (key == Constants::Config::KEY_POS_NUM_SINGLE) {
                currentAction->positionNumberSingleFile = value.toInt();
            } else if (key == Constants::Config::KEY_POS_NUM_MULTI) {
                currentAction->positionNumberMultiFiles = value.toInt();
            } else if (key == Constants::Config::KEY_EXEC) {
                currentAction->execCommand = value;
            } else if (key == Constants::Config::KEY_SEPARATOR) {
                currentAction->separator = value;
                if (value == Constants::Config::SEPARATOR_TOP) {
                    currentAction->separatorTop = true;
                    currentAction->separatorBottom = false;
                } else if (value == Constants::Config::SEPARATOR_BOTTOM) {
                    currentAction->separatorBottom = true;
                    currentAction->separatorTop = false;
                }
            }
        }
    }

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
    if (!data.actionMap.contains(Constants::Defaults::ROOT_ACTION_ID)) {
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
    QSet<QString> visited;
    QList<QSharedPointer<MenuActionItem>> queue;

    if (data.actionMap.contains(Constants::Defaults::ROOT_ACTION_ID)) {
        QSharedPointer<MenuActionItem> root = data.actionMap[Constants::Defaults::ROOT_ACTION_ID];
        root->level = Constants::Defaults::ROOT_LEVEL;
        queue.append(root);
        visited.insert(Constants::Defaults::ROOT_ACTION_ID);
    }

    while (!queue.isEmpty()) {
        QSharedPointer<MenuActionItem> parent = queue.takeFirst();

        for (const QString &childId : parent->childActions) {
            if (data.actionMap.contains(childId) && !visited.contains(childId)) {
                QSharedPointer<MenuActionItem> child = data.actionMap[childId];
                child->level = parent->level + 1;
                if (child->level > 3) {
                    child->level = 3;
                }
                queue.append(child);
                visited.insert(childId);
            }
        }
    }

    LOG_DEBUG(QString("buildTreeStructure: Calculated levels for %1 items").arg(visited.size()));
}

void ConfigParser::calculateLevel(MenuActionItem &action, ConfigData &data) {
    // 这个方法已经不再使用，被 buildTreeStructure 中的 BFS 替代
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
