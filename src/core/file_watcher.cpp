#include "file_watcher.h"

FileWatcher::FileWatcher(QObject *parent)
    : QObject(parent)
{
}

void FileWatcher::watchDirectory(const QString &path) {
    Q_UNUSED(path)
    // TODO: 实现文件监控
}

void FileWatcher::unwatchDirectory(const QString &path) {
    Q_UNUSED(path)
    // TODO: 实现取消监控
}
