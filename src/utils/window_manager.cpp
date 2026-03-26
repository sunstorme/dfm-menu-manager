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

void WindowManager::saveState(QQuickWindow *window) {
    if (!window) {
        return;
    }
    
    m_settings.setValue("window/geometry", window->geometry());
    m_settings.setValue("window/visibility", window->visibility());
}

void WindowManager::restoreState(QQuickWindow *window) {
    if (!window) {
        return;
    }
    
    QRect geometry = m_settings.value("window/geometry").toRect();
    if (!geometry.isEmpty()) {
        window->setGeometry(geometry);
    }
    
    QWindow::Visibility visibility = static_cast<QWindow::Visibility>(
        m_settings.value("window/visibility", QWindow::Windowed).toInt()
    );
    window->setVisibility(visibility);
}
