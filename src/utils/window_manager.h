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
    
    Q_INVOKABLE void saveState(QQuickWindow *window);
    Q_INVOKABLE void restoreState(QQuickWindow *window);
    
private:
    explicit WindowManager(QObject *parent = nullptr);
    
    QSettings m_settings;
    
    struct WindowState {
        QByteArray geometry;
        QList<int> splitterSizes;
    };
};

#endif // WINDOWMANAGER_H
