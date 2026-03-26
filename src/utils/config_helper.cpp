// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "config_helper.h"
#include <QFileInfo>

bool ConfigHelper::isValidConfigFile(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    return fileInfo.exists() && fileInfo.suffix() == "conf";
}
