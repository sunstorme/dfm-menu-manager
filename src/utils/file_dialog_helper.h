// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef FILEDIALOGHELPER_H
#define FILEDIALOGHELPER_H

#include <QObject>
#include <QSettings>

/**
 * @brief 文件选择对话框辅助类
 *
 * 封装 QFileDialog，提供 QML 可调用的文件选择功能，
 * 并按 settingsKey 独立记忆各场景的上次打开目录。
 *
 * QML 用法：
 *   var path = fileDialogHelper.openFile(
 *       qsTr("Select File"),
 *       ["All Files (*)", "Bash Scripts (*.sh)"],
 *       "execCommand"
 *   );
 *   if (path !== "") { ... }
 */
class FileDialogHelper : public QObject {
    Q_OBJECT

public:
    explicit FileDialogHelper(QObject *parent = nullptr);

    /**
     * @brief 打开单文件选择对话框
     * @param title       对话框标题
     * @param nameFilters 文件类型过滤器列表，如 ["All Files (*)", "Scripts (*.sh)"]
     * @param settingsKey 用于记忆上次目录的 key，不同场景用不同 key
     * @return 选择的文件路径，取消则返回空字符串
     */
    Q_INVOKABLE QString openFile(const QString &title = QString(),
                                  const QStringList &nameFilters = QStringList(),
                                  const QString &settingsKey = QStringLiteral("default"));

private:
    QSettings m_settings;
};

#endif // FILEDIALOGHELPER_H
