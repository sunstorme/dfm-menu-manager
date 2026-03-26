// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "menu_file_model.h"
#include "../utils/file_utils.h"
#include <QDir>
#include <QFileInfo>

MenuFileModel::MenuFileModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_showSystemOnly(false)
{
    // 自动加载文件列表
    refresh();
}

int MenuFileModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return m_files.size();
}

QVariant MenuFileModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_files.size()) {
        return QVariant();
    }
    
    const FileInfo &file = m_files.at(index.row());
    
    switch (role) {
    case FileNameRole:
        return file.name;
    case FilePathRole:
        return file.path;
    case IsSystemRole:
        return file.isSystem;
    case IsModifiedRole:
        return file.isModified;
    case CommentRole:
        return file.comment;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MenuFileModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[FileNameRole] = "fileName";
    roles[FilePathRole] = "filePath";
    roles[IsSystemRole] = "isSystem";
    roles[IsModifiedRole] = "isModified";
    roles[CommentRole] = "comment";
    return roles;
}

void MenuFileModel::refresh() {
    beginResetModel();
    m_allFiles.clear();
    
    // 根据showSystemOnly属性决定加载哪些文件
    if (!m_showSystemOnly) {
        // 加载用户配置文件
        QString userDir = FileUtils::getUserConfigDir();
        QDir userConfigDir(userDir);
        if (userConfigDir.exists()) {
            QStringList userFiles = userConfigDir.entryList(QStringList() << "*.conf", QDir::Files);
            for (const QString &fileName : userFiles) {
                FileInfo info;
                info.name = fileName;
                info.path = userDir + "/" + fileName;
                info.isSystem = false;
                info.isModified = false;
                m_allFiles.append(info);
            }
        }
    }
    
    if (m_showSystemOnly) {
        // 加载系统配置文件
        QString systemDir = FileUtils::getSystemConfigDir();
        QDir systemConfigDir(systemDir);
        if (systemConfigDir.exists()) {
            QStringList systemFiles = systemConfigDir.entryList(QStringList() << "*.conf", QDir::Files);
            for (const QString &fileName : systemFiles) {
                FileInfo info;
                info.name = fileName;
                info.path = systemDir + "/" + fileName;
                info.isSystem = true;
                info.isModified = false;
                m_allFiles.append(info);
            }
        }
    }
    
    // 应用搜索过滤
    applySearchFilter();
    
    endResetModel();
}

void MenuFileModel::setSearchFilter(const QString &filter) {
    if (m_searchFilter != filter) {
        m_searchFilter = filter;
        emit searchFilterChanged();
        applySearchFilter();
    }
}

void MenuFileModel::applySearchFilter() {
    beginResetModel();
    m_files.clear();
    
    if (m_searchFilter.isEmpty()) {
        // 如果搜索过滤为空，显示所有文件
        m_files = m_allFiles;
    } else {
        // 根据搜索过滤文件
        QString filterLower = m_searchFilter.toLower();
        for (const FileInfo &info : m_allFiles) {
            if (info.name.toLower().contains(filterLower)) {
                m_files.append(info);
            }
        }
    }
    
    endResetModel();
}

void MenuFileModel::createFile(const QString &name) {
    Q_UNUSED(name)
    // TODO: 实现创建文件
}

void MenuFileModel::deleteFile(const QString &path) {
    Q_UNUSED(path)
    // TODO: 实现删除文件
}

void MenuFileModel::renameFile(const QString &path, const QString &newName) {
    Q_UNUSED(path)
    Q_UNUSED(newName)
    // TODO: 实现重命名文件
}

QString MenuFileModel::copyFile(const QString &sourcePath, bool toSystem) {
    Q_UNUSED(sourcePath)
    Q_UNUSED(toSystem)
    // TODO: 实现复制文件
    return QString();
}
