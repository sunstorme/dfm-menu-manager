// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "file_dialog_helper.h"
#include <QFileDialog>
#include <QStandardPaths>

FileDialogHelper::FileDialogHelper(QObject *parent)
    : QObject(parent)
    , m_settings("deepin", "dfm-menu-manager")
{
}

QString FileDialogHelper::openFile(const QString &title,
                                    const QStringList &nameFilters,
                                    const QString &settingsKey)
{
    // 读取上次打开的目录，默认用户家目录
    const QString settingsPath = QStringLiteral("FileDialog/%1").arg(settingsKey);
    QString dir = m_settings.value(settingsPath,
                                    QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
                    .toString();

    QWidget *parentWidget = nullptr;
    // 尝试获取当前活动窗口作为父窗口
    if (parent()) {
        parentWidget = qobject_cast<QWidget*>(parent());
    }

    QString filePath = QFileDialog::getOpenFileName(parentWidget, title, dir, nameFilters.join(QStringLiteral(";;")));

    // 记忆本次打开的目录
    if (!filePath.isEmpty()) {
        QString newDir = QFileInfo(filePath).absolutePath();
        m_settings.setValue(settingsPath, newDir);
        m_settings.sync();
    }

    return filePath;
}
