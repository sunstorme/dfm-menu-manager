#include "config_writer.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

bool ConfigWriter::writeToFile(const QString &filePath, 
                               const ConfigParser::ConfigData &data) {
    // 先备份
    if (!backupFile(filePath)) {
        qWarning() << "备份文件失败:" << filePath;
        return false;
    }
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件进行写入:" << filePath;
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // 写入 Menu Entry 组
    out << "# Configuration file group entry\n";
    out << "[Menu Entry]\n";
    out << "\t\t\t\t\t\t\t\n";
    
    // 写入描述
    if (!data.comment.isEmpty()) {
        out << "  # Description of this .conf file\n";
        out << "Comment=" << data.comment << "\n";
    }
    
    if (!data.commentLocal.isEmpty()) {
        out << "  # Description of this .conf file in Chinese locale\n";
        out << "Comment[zh_CN]=" << data.commentLocal << "\n";
    }
    
    // 写入版本号
    out << "  # Protocol version used by this configuration file\n";
    out << "Version=" << data.version << "\n";
    out << "\n";
    
    // 写入根菜单的 Actions
    auto rootItem = data.actionMap.value("root");
    if (rootItem && !rootItem->childActions.isEmpty()) {
        out << "  # Contains " << rootItem->childActions.size() 
            << " groups under [Menu Entry]\n";
        out << "Actions=" << rootItem->childActions.join(":") << "\n";
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
    
    QString backupPath = filePath + ".bak";
    
    // 如果备份文件已存在,先删除
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }
    
    return QFile::copy(filePath, backupPath);
}

QString ConfigWriter::formatEntry(const MenuActionItem &item) {
    QString result;
    QTextStream out(&result);
    
    out << "[Menu Action " << item.id << "]\n";
    out << "Name=" << item.name << "\n";
    
    if (!item.nameLocal.isEmpty()) {
        out << "Name[zh_CN]=" << item.nameLocal << "\n";
    }
    
    // 菜单类型
    if (!item.menuTypes.isEmpty()) {
        out << "X-DFM-MenuTypes=" << item.menuTypes.join(":") << "\n";
    }
    
    // 文件后缀
    if (!item.supportSuffix.isEmpty()) {
        out << "X-DFM-SupportSuffix=" << item.supportSuffix.join(":") << "\n";
    }
    
    // 位置
    out << "PosNum=" << item.positionNumber << "\n";
    
    // 分隔符
    if (item.separatorTop) {
        out << "Separator=Top\n";
    }
    if (item.separatorBottom) {
        out << "Separator=Bottom\n";
    }
    
    // 子菜单或执行命令
    if (!item.childActions.isEmpty()) {
        out << "Actions=" << item.childActions.join(":") << "\n";
    } else if (!item.execCommand.isEmpty()) {
        out << "Exec=" << item.execCommand << "\n";
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
