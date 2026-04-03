// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "file_utils.h"
#include <QDir>
#include <QStandardPaths>
#include <QString>
#include <QStringList>

QString FileUtils::getUserConfigDir() {
    return QDir::homePath() + "/.local/share/deepin/dde-file-manager/context-menus";
}

QString FileUtils::getSystemConfigDir() {
    return "/usr/share/applications/context-menus";
}

bool FileUtils::ensureDirExists(const QString &path) {
    QDir dir(path);
    if (!dir.exists()) {
        return dir.mkpath(path);
    }
    return true;
}
