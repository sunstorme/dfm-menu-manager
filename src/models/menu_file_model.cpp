// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "menu_file_model.h"
#include "../core/file_watcher.h"
#include "../utils/file_utils.h"
#include "../utils/logger.h"
#include <QDir>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QTimer>

MenuFileModel::MenuFileModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_showSystemOnly(false)
    , m_fileWatcher(nullptr)
{
    // 自动加载文件列表
    refresh();
    
    // 设置文件监控
    setupFileWatcher();
}

MenuFileModel::~MenuFileModel() {
    cleanupFileWatcher();
}

void MenuFileModel::setupFileWatcher() {
    if (!m_fileWatcher) {
        m_fileWatcher = new FileWatcher(this);
        
        connect(m_fileWatcher, &FileWatcher::fileChanged,
                this, &MenuFileModel::onFileChanged);
        connect(m_fileWatcher, &FileWatcher::directoryChanged,
                this, &MenuFileModel::onDirectoryChanged);
    }
    
    // 监控用户配置目录
    QString userDir = FileUtils::getUserConfigDir();
    if (QDir(userDir).exists()) {
        m_fileWatcher->watchDirectory(userDir);
        LOG_DEBUG(QString("Watching user directory: %1").arg(userDir));
    }

    // 监控系统配置目录
    QString systemDir = FileUtils::getSystemConfigDir();
    if (QDir(systemDir).exists()) {
        m_fileWatcher->watchDirectory(systemDir);
        LOG_DEBUG(QString("Watching system directory: %1").arg(systemDir));
    }
}

void MenuFileModel::cleanupFileWatcher() {
    if (m_fileWatcher) {
        QString userDir = FileUtils::getUserConfigDir();
        QString systemDir = FileUtils::getSystemConfigDir();
        
        m_fileWatcher->unwatchDirectory(userDir);
        m_fileWatcher->unwatchDirectory(systemDir);
    }
}

void MenuFileModel::onFileChanged(const QString &path) {
    LOG_DEBUG(QString("File changed, refreshing model: %1").arg(path));
    // 延迟刷新以避免频繁更新
    QTimer::singleShot(100, this, [this]() {
        refresh();
    });
}

void MenuFileModel::onDirectoryChanged(const QString &path) {
    LOG_DEBUG(QString("Directory changed, refreshing model: %1").arg(path));
    // 延迟刷新以避免频繁更新
    QTimer::singleShot(100, this, [this]() {
        refresh();
    });
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
    if (name.isEmpty()) {
        LOG_WARNING("Cannot create file with empty name");
        return;
    }

    // 确定目标目录
    QString targetDir;
    if (m_showSystemOnly) {
        targetDir = FileUtils::getSystemConfigDir();
    } else {
        targetDir = FileUtils::getUserConfigDir();
    }

    // 确保目录存在
    if (!FileUtils::ensureDirExists(targetDir)) {
        LOG_WARNING(QString("Failed to create directory: %1").arg(targetDir));
        return;
    }

    // 构建文件路径
    QString filePath = targetDir + "/" + name;
    if (!filePath.endsWith(".conf")) {
        filePath += ".conf";
    }

    // 检查文件是否已存在
    if (QFile::exists(filePath)) {
        LOG_WARNING(QString("File already exists: %1").arg(filePath));
        return;
    }

    // 创建空文件
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write("# DFM Context Menu Configuration\n");
        file.close();

        // 重新启动文件监视以包含新文件
        QTimer::singleShot(50, this, [this]() {
            setupFileWatcher();
        });

        // 刷新模型以显示新文件
        QTimer::singleShot(100, this, [this]() {
            refresh();
        });
    } else {
        LOG_WARNING(QString("Failed to create file: %1").arg(filePath));
    }
}

void MenuFileModel::deleteFile(const QString &path) {
    if (path.isEmpty()) {
        LOG_WARNING("Cannot delete file with empty path");
        return;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        LOG_WARNING(QString("File does not exist: %1").arg(path));
        return;
    }

    // 临时停止文件监视，以便删除操作
    if (m_fileWatcher) {
        m_fileWatcher->unwatchDirectory(fileInfo.absolutePath());
    }

    // 删除文件
    QFile file(path);
    if (file.remove()) {
        LOG_DEBUG(QString("Successfully deleted file: %1").arg(path));

        // 重新启动文件监视
        QTimer::singleShot(50, this, [this]() {
            setupFileWatcher();
        });

        // 刷新模型以更新显示
        QTimer::singleShot(100, this, [this]() {
            refresh();
        });
    } else {
        qWarning() << "Failed to delete file:" << path
                   << "- Error:" << file.errorString();
        // 即使失败也要重新启动监视
        QTimer::singleShot(50, this, [this]() {
            setupFileWatcher();
        });
    }
}

void MenuFileModel::renameFile(const QString &path, const QString &newName) {
    if (path.isEmpty() || newName.isEmpty()) {
        LOG_WARNING("Cannot rename file with empty path or name");
        return;
    }

    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        LOG_WARNING(QString("File does not exist: %1").arg(path));
        return;
    }

    // 构建新文件名
    QString newFileName = newName;
    if (!newFileName.endsWith(".conf")) {
        newFileName += ".conf";
    }
    
    // 构建新文件路径
    QString newPath = fileInfo.absolutePath() + "/" + newFileName;
    
    // 检查目标文件是否已存在
    if (QFile::exists(newPath) && newPath != path) {
        LOG_WARNING(QString("Target file already exists: %1").arg(newPath));
        return;
    }

    // 临时停止文件监视，以便重命名操作
    if (m_fileWatcher) {
        m_fileWatcher->unwatchDirectory(fileInfo.absolutePath());
    }

    // 重命名文件
    QFile file(path);
    if (file.rename(newPath)) {
        LOG_DEBUG(QString("Successfully renamed file from %1 to %2").arg(path).arg(newPath));

        // 重新启动文件监视
        QTimer::singleShot(50, this, [this]() {
            setupFileWatcher();
        });

        // 刷新模型以更新显示
        QTimer::singleShot(100, this, [this]() {
            refresh();
        });
    } else {
        qWarning() << "Failed to rename file from" << path << "to" << newPath
                   << "- Error:" << file.errorString();
        // 即使失败也要重新启动监视
        QTimer::singleShot(50, this, [this]() {
            setupFileWatcher();
        });
    }
}

QString MenuFileModel::copyFile(const QString &sourcePath, bool toSystem) {
    Q_UNUSED(sourcePath)
    Q_UNUSED(toSystem)
    // TODO: 实现复制文件
    return QString();
}

void MenuFileModel::openFile(const QString &path) {
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        LOG_WARNING(QString("File does not exist: %1").arg(path));
        return;
    }

    LOG_DEBUG(QString("Attempting to open file: %1").arg(path));

    // 使用Qt的QDesktopServices打开文件（跨平台方式）
    QUrl url = QUrl::fromLocalFile(path);
    bool success = QDesktopServices::openUrl(url);

    if (success) {
        LOG_DEBUG(QString("Successfully opened file with Qt: %1").arg(path));
    } else {
        LOG_WARNING(QString("Failed to open file with Qt: %1").arg(path));
    }
}

void MenuFileModel::openContainingFolder(const QString &path) {
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        LOG_WARNING(QString("File does not exist: %1").arg(path));
        return;
    }

    QString folderPath = fileInfo.absolutePath();
    LOG_DEBUG(QString("Attempting to open containing folder: %1").arg(folderPath));

    // 首先尝试使用dde-file-manager打开文件夹并选中文件（深度定制功能）
    QStringList ddeArgs;
    ddeArgs << "--show-item" << path;

    qint64 ddePid = 0;
    bool ddeSuccess = QProcess::startDetached("dde-file-manager", ddeArgs, QString(), &ddePid);

    if (ddeSuccess && ddePid > 0) {
        LOG_DEBUG(QString("Successfully started dde-file-manager for: %1 PID: %2").arg(path).arg(ddePid));
        return;
    } else {
        LOG_DEBUG(QString("dde-file-manager not available, using Qt to open folder: %1").arg(folderPath));
    }

    // 如果dde-file-manager不可用，使用Qt的QDesktopServices打开文件夹
    QUrl url = QUrl::fromLocalFile(folderPath);
    bool success = QDesktopServices::openUrl(url);

    if (success) {
        LOG_DEBUG(QString("Successfully opened folder with Qt: %1").arg(folderPath));
    } else {
        LOG_WARNING(QString("Failed to open folder with Qt: %1").arg(folderPath));
    }
}

void MenuFileModel::startNewFile() {
    beginInsertRows(QModelIndex(), 0, 0);
    
    FileInfo newFileInfo;
    newFileInfo.name = "";
    newFileInfo.path = "";
    newFileInfo.isSystem = false;
    newFileInfo.isModified = false;
    newFileInfo.comment = "";
    
    m_files.prepend(newFileInfo);
    m_allFiles.prepend(newFileInfo);
    
    endInsertRows();
}

void MenuFileModel::cancelNewFile() {
    if (!m_files.isEmpty() && m_files.first().path.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, 0);
        m_files.removeFirst();
        m_allFiles.removeFirst();
        endRemoveRows();
    }
}
