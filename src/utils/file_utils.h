// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils {
public:
    static QString getUserConfigDir();
    static QString getSystemConfigDir();
    static bool ensureDirExists(const QString &path);
};

#endif // FILEUTILS_H
