// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>

class FileWatcher : public QObject {
    Q_OBJECT
    
public:
    explicit FileWatcher(QObject *parent = nullptr);
    
public slots:
    void watchDirectory(const QString &path);
    void unwatchDirectory(const QString &path);
    
signals:
    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);
};

#endif // FILEWATCHER_H
