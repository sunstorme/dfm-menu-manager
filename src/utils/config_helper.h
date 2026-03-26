// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H

#include <QString>

class ConfigHelper {
public:
    static bool isValidConfigFile(const QString &filePath);
};

#endif // CONFIGHELPER_H
