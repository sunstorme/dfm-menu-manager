// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QObject>
#include <QQuickWindow>
#include <QSettings>

/**
 * @brief 窗口状态管理器
 * 
 * 负责保存和恢复窗口的状态(位置、大小、列宽等)
 */
class WindowManager : public QObject {
    Q_OBJECT
    
public:
    static WindowManager* instance();
    
    Q_INVOKABLE void saveState(QQuickWindow *window, qreal filePanelWidth, qreal menuEditorWidth, qreal propertyPanelWidth);
    Q_INVOKABLE void restoreState(QQuickWindow *window, QObject *splitView);
    
private:
    explicit WindowManager(QObject *parent = nullptr);
    
    QSettings m_settings;
    
    struct WindowState {
        QByteArray geometry;
        QList<int> splitterSizes;
    };
};

#endif // WINDOWMANAGER_H
