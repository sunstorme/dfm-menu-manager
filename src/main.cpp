/*
 * dfm-menu-manager - DFM right-click menu manager
 * Copyright (C) 2025 zhanghongyuan <zhanghongyuan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "utils/constants.h"
#include <QQmlContext>
#include "utils/logger.h"
#include <QTranslator>
#include <QLocale>
#include "core/menu_manager.h"
#include "core/file_type_manager.h"
#include "models/menu_tree_model.h"
#include "models/menu_file_model.h"
#include "utils/window_manager.h"

int main(int argc, char *argv[])
{
    LOG_INFO("=== DFM Menu Manager Starting ===");

    // 设置高DPI属性(必须在创建QGuiApplication之前)
    LOG_DEBUG("Setting High DPI attributes...");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    LOG_DEBUG("Creating QGuiApplication...");
    QGuiApplication app(argc, argv);
    app.setApplicationName("dfm-menu-manager");
    app.setApplicationVersion(Constants::Defaults::DEFAULT_APP_VERSION);
    app.setOrganizationName("deepin");
    LOG_DEBUG("QGuiApplication created successfully");

    // 加载翻译
    LOG_DEBUG("Loading translations...");
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "dfm-menu-manager_" + QLocale(locale).name();
        LOG_DEBUG(QString("Trying to load translation: %1").arg(baseName));

        // 尝试从多个路径加载翻译文件
        QStringList searchPaths;
        searchPaths << ":/i18n/translations/"
                    << QCoreApplication::applicationDirPath() + "/translations/"
                    << QCoreApplication::applicationDirPath() + "/../share/dfm-menu-manager/translations/";

        bool loaded = false;
        for (const QString &path : searchPaths) {
            LOG_DEBUG(QString("  Trying path: %1").arg(path + baseName + ".qm"));
            if (translator.load(path + baseName + ".qm")) {
                app.installTranslator(&translator);
                LOG_DEBUG(QString("Successfully loaded translation from: %1").arg(path + baseName + ".qm"));
                loaded = true;
                break;
            }
        }

        if (loaded) {
            break;
        }
    }
    
    // 注册QML类型
    LOG_DEBUG("Registering QML types...");
    qmlRegisterType<MenuTreeModel>(Constants::QML_MODULE_NAME, Constants::QML_MAJOR_VERSION, Constants::QML_MINOR_VERSION, "MenuTreeModel");
    qmlRegisterType<MenuFileModel>(Constants::QML_MODULE_NAME, Constants::QML_MAJOR_VERSION, Constants::QML_MINOR_VERSION, "MenuFileModel");
    qmlRegisterType<MenuManager>(Constants::QML_MODULE_NAME, Constants::QML_MAJOR_VERSION, Constants::QML_MINOR_VERSION, "MenuManager");
    qmlRegisterType<FileTypeManager>(Constants::QML_MODULE_NAME, Constants::QML_MAJOR_VERSION, Constants::QML_MINOR_VERSION, "FileTypeManager");
    qmlRegisterSingletonType<WindowManager>(Constants::QML_MODULE_NAME, Constants::QML_MAJOR_VERSION, Constants::QML_MINOR_VERSION, "WindowManager",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject* {
            Q_UNUSED(scriptEngine)
            WindowManager* instance = WindowManager::instance();
            // 设置对象所有权为 C++，防止 QML 引擎删除它
            QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
            return instance;
        }
    );
    LOG_DEBUG("QML types registered");

    // 创建管理器
    LOG_DEBUG("Creating MenuManager...");
    MenuManager menuManager;
    LOG_DEBUG("Loading configurations...");
    menuManager.loadConfigurations();
    LOG_DEBUG("Configurations loaded");

    // 创建文件类型管理器
    LOG_DEBUG("Creating FileTypeManager...");
    FileTypeManager fileTypeManager;
    LOG_DEBUG("FileTypeManager created");

    // QML引擎
    LOG_DEBUG("Creating QML engine...");
    QQmlApplicationEngine engine;

    // 添加qmldir路径
    LOG_DEBUG("Adding import paths...");
    engine.addImportPath(":/qml/styles");

    // 暴露管理器到QML
    LOG_DEBUG("Setting context properties...");
    engine.rootContext()->setContextProperty("menuManager", &menuManager);
    engine.rootContext()->setContextProperty("fileTypeManager", &fileTypeManager);

    // 加载主QML文件
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    LOG_DEBUG(QString("Loading QML file: %1").arg(url.toString()));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        LOG_DEBUG(QString("QML object created: %1 URL: %2").arg(quintptr(obj), 0, 16).arg(objUrl.toString()));
        if (!obj && url == objUrl) {
            LOG_WARNING("Failed to create QML object, exiting...");
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);

    engine.load(url);
    LOG_DEBUG("QML load initiated");

    LOG_INFO("=== Entering event loop ===");
    return app.exec();
}

