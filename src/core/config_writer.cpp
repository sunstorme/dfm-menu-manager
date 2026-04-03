// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "config_writer.h"
#include "../utils/logger.h"
#include "../utils/constants.h"
#include <QFile>
#include <QTextStream>
#include <QDir>

bool ConfigWriter::writeToFile(const QString &filePath,
                               const ConfigParser::ConfigData &data) {
    // 先备份
    if (!backupFile(filePath)) {
        LOG_WARNING(QString("备份文件失败: %1").arg(filePath));
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG_WARNING(QString("无法打开文件进行写入: %1").arg(filePath));
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入 Menu Entry 组
    out << "# Configuration file group entry\n";
    out << "[" << Constants::Config::MENU_ENTRY_GROUP << "]\n";
    out << "\t\t\t\t\t\t\t\n";

    // 写入描述
    if (!data.comment.isEmpty()) {
        out << "  # Description of this " << Constants::File::CONFIG_EXTENSION << " file\n";
        out << Constants::Config::KEY_COMMENT << "=" << data.comment << "\n";
    }

    if (!data.commentLocal.isEmpty()) {
        out << "  # Description of this " << Constants::File::CONFIG_EXTENSION << " file in Chinese locale\n";
        out << Constants::Config::KEY_COMMENT_LOCAL << "=" << data.commentLocal << "\n";
    }

    // 写入版本号
    out << "  # Protocol version used by this configuration file\n";
    out << Constants::Config::KEY_VERSION << "=" << data.version << "\n";
    out << "\n";
    
    // 写入根菜单的 Actions
    auto rootItem = data.actionMap.value(Constants::Defaults::ROOT_ACTION_ID);
    if (rootItem && !rootItem->childActions.isEmpty()) {
        out << "  # Contains " << rootItem->childActions.size()
            << " groups under [" << Constants::Config::MENU_ENTRY_GROUP << "]\n";
        out << Constants::Config::KEY_ACTIONS << "=" << rootItem->childActions.join(":") << "\n";
        out << "\n";
    }

    // 写入各个 Menu Action 组
    for (const auto &action : data.actions) {
        if (action.isRoot) {
            continue;  // 跳过根项
        }

        out << formatEntry(action) << "\n";
    }
    
    file.close();
    return true;
}

bool ConfigWriter::backupFile(const QString &filePath) {
    if (!QFile::exists(filePath)) {
        return true;  // 文件不存在,不需要备份
    }

    QString backupPath = filePath + Constants::File::BACKUP_EXTENSION;

    // 如果备份文件已存在,先删除
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }

    return QFile::copy(filePath, backupPath);
}

QString ConfigWriter::formatEntry(const MenuActionItem &item) {
    QString result;
    QTextStream out(&result);

    out << "[" << Constants::Config::MENU_ACTION_PREFIX << item.id << "]\n";
    out << Constants::Config::KEY_NAME << "=" << item.name << "\n";

    if (!item.nameLocal.isEmpty()) {
        out << Constants::Config::KEY_NAME_LOCAL << "=" << item.nameLocal << "\n";
    }

    // 菜单类型
    if (!item.menuTypes.isEmpty()) {
        out << Constants::Config::KEY_MENU_TYPES << "=" << item.menuTypes.join(":") << "\n";
    }

    // 文件后缀
    if (!item.supportSuffix.isEmpty()) {
        out << Constants::Config::KEY_SUPPORT_SUFFIX << "=" << item.supportSuffix.join(":") << "\n";
    }

    // 位置
    out << Constants::Config::KEY_POS_NUM << "=" << item.positionNumber << "\n";

    // 单文件位置
    if (item.positionNumberSingleFile > 0) {
        out << Constants::Config::KEY_POS_NUM_SINGLE << "=" << item.positionNumberSingleFile << "\n";
    }

    // 多文件位置
    if (item.positionNumberMultiFiles > 0) {
        out << Constants::Config::KEY_POS_NUM_MULTI << "=" << item.positionNumberMultiFiles << "\n";
    }

    // 分隔符
    if (!item.separator.isEmpty()) {
        out << Constants::Config::KEY_SEPARATOR << "=" << item.separator << "\n";
    } else if (item.separatorTop) {
        out << Constants::Config::KEY_SEPARATOR << "=" << Constants::Config::SEPARATOR_TOP << "\n";
    } else if (item.separatorBottom) {
        out << Constants::Config::KEY_SEPARATOR << "=" << Constants::Config::SEPARATOR_BOTTOM << "\n";
    }

    // 子菜单或执行命令
    if (!item.childActions.isEmpty()) {
        out << Constants::Config::KEY_ACTIONS << "=" << item.childActions.join(":") << "\n";
    } else if (!item.execCommand.isEmpty()) {
        out << Constants::Config::KEY_EXEC << "=" << item.execCommand << "\n";
    }

    return result;
}

QString ConfigWriter::formatComment(const QString &text) {
    return text;
}

QString ConfigWriter::formatList(const QStringList &list, 
                                 const QString &separator) {
    return list.join(separator);
}
