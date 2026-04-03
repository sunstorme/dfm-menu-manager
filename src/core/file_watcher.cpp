// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "file_watcher.h"
#include "../utils/logger.h"
#include <QDir>
#include <QFileInfo>

FileWatcher::FileWatcher(QObject *parent)
    : QObject(parent)
    , m_watcher(new QFileSystemWatcher(this))
{
    // 连接QFileSystemWatcher的信号
    connect(m_watcher, &QFileSystemWatcher::fileChanged,
            this, &FileWatcher::onFileChanged);
    connect(m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &FileWatcher::onDirectoryChanged);
}

FileWatcher::~FileWatcher() {
    // QFileSystemWatcher会自动清理
}

void FileWatcher::watchDirectory(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        LOG_WARNING(QString("Directory does not exist: %1").arg(path));
        return;
    }

    // 如果已经在监控，先移除
    if (m_watcher->directories().contains(path)) {
        m_watcher->removePath(path);
    }

    // 添加目录监控
    if (m_watcher->addPath(path)) {
        LOG_DEBUG(QString("Started watching directory: %1").arg(path));
        // 扫描并监控目录中的文件
        scanDirectoryFiles(path);
    } else {
        LOG_WARNING(QString("Failed to watch directory: %1").arg(path));
    }
}

void FileWatcher::unwatchDirectory(const QString &path) {
    // 移除目录监控
    if (m_watcher->directories().contains(path)) {
        m_watcher->removePath(path);
        LOG_DEBUG(QString("Stopped watching directory: %1").arg(path));
    }
    
    // 移除该目录下所有文件的监控
    if (m_directoryFiles.contains(path)) {
        const QSet<QString> &files = m_directoryFiles[path];
        for (const QString &filePath : files) {
            if (m_watcher->files().contains(filePath)) {
                m_watcher->removePath(filePath);
            }
        }
        m_directoryFiles.remove(path);
    }
}

void FileWatcher::scanDirectoryFiles(const QString &path) {
    QDir dir(path);
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*.conf", QDir::Files);

    QSet<QString> currentFiles;
    for (const QFileInfo &fileInfo : fileList) {
        QString filePath = fileInfo.absoluteFilePath();
        currentFiles.insert(filePath);

        // 如果文件还没有被监控，添加监控
        if (!m_watcher->files().contains(filePath)) {
            if (m_watcher->addPath(filePath)) {
                LOG_DEBUG(QString("Started watching file: %1").arg(filePath));
            }
        }
    }

    // 更新目录文件映射
    m_directoryFiles[path] = currentFiles;
}

void FileWatcher::onFileChanged(const QString &path) {
    LOG_DEBUG(QString("File changed: %1").arg(path));
    emit fileChanged(path);
}

void FileWatcher::onDirectoryChanged(const QString &path) {
    LOG_DEBUG(QString("Directory changed: %1").arg(path));

    // 重新扫描目录中的文件
    QSet<QString> oldFiles = m_directoryFiles.value(path);
    scanDirectoryFiles(path);
    QSet<QString> newFiles = m_directoryFiles.value(path);

    // 检测新增的文件
    QSet<QString> addedFiles = newFiles - oldFiles;
    for (const QString &filePath : addedFiles) {
        LOG_DEBUG(QString("File added: %1").arg(filePath));
        emit fileChanged(filePath);
    }

    // 检测删除的文件
    QSet<QString> removedFiles = oldFiles - newFiles;
    for (const QString &filePath : removedFiles) {
        LOG_DEBUG(QString("File removed: %1").arg(filePath));
        // 移除文件监控
        if (m_watcher->files().contains(filePath)) {
            m_watcher->removePath(filePath);
        }
        emit fileChanged(filePath);
    }

    emit directoryChanged(path);
}
