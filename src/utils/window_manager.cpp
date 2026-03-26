// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "window_manager.h"

WindowManager::WindowManager(QObject *parent)
    : QObject(parent)
    , m_settings("deepin", "dfm-menu-manager")
{
}

WindowManager* WindowManager::instance() {
    static WindowManager* manager = nullptr;
    if (!manager) {
        manager = new WindowManager();
    }
    return manager;
}

void WindowManager::saveState(QQuickWindow *window, qreal filePanelWidth, qreal menuEditorWidth, qreal propertyPanelWidth) {
    if (!window) {
        return;
    }
    
    m_settings.setValue("window/geometry", window->geometry());
    
    m_settings.setValue("layout/filePanelWidth", filePanelWidth);
    m_settings.setValue("layout/menuEditorWidth", menuEditorWidth);
    m_settings.setValue("layout/propertyPanelWidth", propertyPanelWidth);
    
    m_settings.sync();
}

void WindowManager::restoreState(QQuickWindow *window, QObject *splitView) {
    if (!window) {
        return;
    }
    
    QRect geometry = m_settings.value("window/geometry").toRect();
    if (!geometry.isEmpty()) {
        window->setGeometry(geometry);
    }
    
    if (splitView) {
        qreal filePanelWidth = m_settings.value("layout/filePanelWidth", 350).toReal();
        qreal menuEditorWidth = m_settings.value("layout/menuEditorWidth", 630).toReal();
        qreal propertyPanelWidth = m_settings.value("layout/propertyPanelWidth", 420).toReal();
        
        splitView->setProperty("filePanelWidth", filePanelWidth);
        splitView->setProperty("menuEditorWidth", menuEditorWidth);
        splitView->setProperty("propertyPanelWidth", propertyPanelWidth);
    }
}
