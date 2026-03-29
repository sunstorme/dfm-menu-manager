// SPDX-FileCopyrightText: 2026 zhanghongyuan <zhanghongyuan@uniontech.com>
// SPDX-License-Identifier: GPL-3.0-or-later
#include "menu_manager.h"
#include "file_watcher.h"
#include "../utils/file_utils.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

MenuManager::MenuManager(QObject *parent)
    : QObject(parent)
    , m_watcher(new FileWatcher(this))
    , m_fileModel(new MenuFileModel(this))
{
    // 连接文件监控信号，当外部编辑文件时自动刷新
    connect(m_watcher, &FileWatcher::fileChanged, 
            this, &MenuManager::onFileChanged);
    connect(m_watcher, &FileWatcher::directoryChanged,
            this, &MenuManager::onDirectoryChanged);
}

void MenuManager::loadConfigurations() {
    // 确保目录存在
    FileUtils::ensureDirExists(FileUtils::getUserConfigDir());
    
    // 刷新文件列表
    m_fileModel->refresh();
    
    // 监控目录变化
    m_watcher->watchDirectory(FileUtils::getUserConfigDir());
    m_watcher->watchDirectory(FileUtils::getSystemConfigDir());
}

bool MenuManager::saveConfiguration(const QString &filePath) {
    if (!m_models.contains(filePath)) {
        emit errorOccurred("未加载的配置文件");
        return false;
    }
    
    MenuTreeModel *model = m_models[filePath];
    if (!model) {
        emit errorOccurred("无效的模型");
        return false;
    }
    
    // 设置保存标志，避免触发文件监控的重新加载
    m_isSaving = true;
    
    // 临时断开文件监控信号，避免触发MenuFileModel的刷新
    disconnect(m_watcher, &FileWatcher::fileChanged,
               this, &MenuManager::onFileChanged);
    disconnect(m_watcher, &FileWatcher::directoryChanged,
               this, &MenuManager::onDirectoryChanged);
    
    // 从模型获取配置数据并保存
    ConfigParser::ConfigData data = model->getConfigData();
    bool success = m_writer.writeToFile(filePath, data);
    
    // 延迟重置保存标志并重新连接信号
    QTimer::singleShot(200, this, [this]() {
        m_isSaving = false;
        // 重新连接文件监控信号
        connect(m_watcher, &FileWatcher::fileChanged,
                this, &MenuManager::onFileChanged);
        connect(m_watcher, &FileWatcher::directoryChanged,
                this, &MenuManager::onDirectoryChanged);
    });
    
    if (!success) {
        emit errorOccurred("保存配置文件失败");
        return false;
    }
    
    emit configSaved(filePath);
    return true;
}

bool MenuManager::saveCurrentModel() {
    if (m_currentConfig.isEmpty()) {
        emit errorOccurred("没有选中的配置文件");
        return false;
    }
    
    return saveConfiguration(m_currentConfig);
}

bool MenuManager::createNewConfig(const QString &name, bool isSystem) {
    QString dir = isSystem ? FileUtils::getSystemConfigDir() 
                           : FileUtils::getUserConfigDir();
    QString filePath = dir + "/" + name;
    
    if (QFile::exists(filePath)) {
        emit errorOccurred("配置文件已存在");
        return false;
    }
    
    // 创建默认配置
    ConfigParser::ConfigData data;
    data.version = "1.0";
    data.comment = "New configuration";
    data.commentLocal = "新配置";
    data.rootActionId = "root";
    
    // 创建根菜单项
    MenuActionItem rootItem;
    rootItem.id = "root";
    rootItem.isRoot = true;
    rootItem.level = 0;
    rootItem.configFile = filePath;
    rootItem.isSystem = isSystem;
    data.actions.append(rootItem);
    data.actionMap["root"] = &data.actions.last();
    
    // 写入文件
    if (!m_writer.writeToFile(filePath, data)) {
        emit errorOccurred("创建配置文件失败");
        return false;
    }
    
    m_fileModel->refresh();
    emit configLoaded(filePath);
    return true;
}

bool MenuManager::deleteConfig(const QString &filePath) {
    if (QFile::exists(filePath)) {
        if (!QFile::remove(filePath)) {
            emit errorOccurred("删除配置文件失败");
            return false;
        }
        
        m_models.remove(filePath);
        m_fileModel->refresh();
        emit configChanged(filePath);
        return true;
    }
    
    emit errorOccurred("配置文件不存在");
    return false;
}

MenuTreeModel* MenuManager::getMenuModel(const QString &configFile) {
    qDebug() << "MenuManager::getMenuModel called with:" << configFile;
    if (!m_models.contains(configFile)) {
        qDebug() << "Creating new model for:" << configFile;
        // 解析配置文件
        ConfigParser::ConfigData data = m_parser.parseFile(configFile);
        
        qDebug() << "Config data isValid:" << data.isValid();
        qDebug() << "Config data actions count:" << data.actions.size();
        
        // 创建模型（即使配置数据无效，也创建空模型让用户可以编辑）
        MenuTreeModel *model = new MenuTreeModel(this);
        if (data.isValid()) {
            model->setConfigData(data);
        } else {
            qDebug() << "Config data is invalid, creating empty model for editing";
            // 不发射错误信号，因为空配置文件是允许的
            // emit errorOccurred("解析配置文件失败");
        }
        m_models[configFile] = model;
        qDebug() << "Model created, rowCount:" << model->rowCount();
    }
    
    return m_models[configFile];
}

void MenuManager::setCurrentConfig(const QString &configFile) {
    qDebug() << "MenuManager::setCurrentConfig called with:" << configFile;
    m_currentConfig = configFile;
    emit configLoaded(configFile);
}

QString MenuManager::getCurrentConfig() const {
    return m_currentConfig;
}

bool MenuManager::validateConfig(const QString &filePath) {
    ConfigParser::ConfigData data = m_parser.parseFile(filePath);
    return m_parser.validate(data);
}

QStringList MenuManager::getValidationErrors() {
    // TODO: 实现获取验证错误
    return QStringList();
}

MenuFileModel* MenuManager::getFileModel() {
    return m_fileModel;
}

QString MenuManager::exportToJson(const QString &configFile) {
    // 解析配置文件
    ConfigParser::ConfigData data = m_parser.parseFile(configFile);
    
    if (!data.isValid()) {
        qWarning() << "Failed to parse config file:" << configFile;
        return "{}";
    }
    
    // 构建 JSON 对象
    QJsonObject rootObj;
    rootObj["version"] = data.version;
    rootObj["comment"] = data.comment;
    rootObj["commentLocal"] = data.commentLocal;
    rootObj["rootActionId"] = data.rootActionId;
    
    // 构建菜单项数组
    QJsonArray actionsArray;
    
    // 使用 BFS 遍历所有菜单项
    QSet<QString> visited;
    QList<const MenuActionItem*> queue;
    
    // 从根节点开始
    if (data.actionMap.contains("root")) {
        queue.append(data.actionMap["root"]);
        visited.insert("root");
    }
    
    while (!queue.isEmpty()) {
        const MenuActionItem* item = queue.takeFirst();
        
        // 将当前项转换为 JSON 对象
        QJsonObject itemObj;
        itemObj["id"] = item->id;
        itemObj["name"] = item->name;
        itemObj["nameLocal"] = item->nameLocal.isEmpty() ? item->name : item->nameLocal;
        itemObj["comment"] = item->comment;
        itemObj["commentLocal"] = item->commentLocal;
        itemObj["level"] = item->level;
        itemObj["isRoot"] = item->isRoot;
        itemObj["isSystem"] = item->isSystem;
        itemObj["positionNumber"] = item->positionNumber;
        itemObj["execCommand"] = item->execCommand;
        itemObj["configFile"] = item->configFile;
        
        // 添加数组字段
        QJsonArray childActionsArray = QJsonArray::fromStringList(item->childActions);
        itemObj["childActions"] = childActionsArray;
        
        QJsonArray menuTypesArray = QJsonArray::fromStringList(item->menuTypes);
        itemObj["menuTypes"] = menuTypesArray;
        
        QJsonArray supportSuffixArray = QJsonArray::fromStringList(item->supportSuffix);
        itemObj["supportSuffix"] = supportSuffixArray;
        
        actionsArray.append(itemObj);
        
        // 将子项添加到队列
        for (const QString &childId : item->childActions) {
            if (data.actionMap.contains(childId) && !visited.contains(childId)) {
                queue.append(data.actionMap[childId]);
                visited.insert(childId);
            }
        }
    }
    
    rootObj["actions"] = actionsArray;
    rootObj["totalActions"] = actionsArray.size();
    
    // 转换为 JSON 字符串
    QJsonDocument doc(rootObj);
    QString jsonString = QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
    
    qDebug() << "Exported config to JSON:" << configFile;
    qDebug() << "Total actions:" << actionsArray.size();
    
    return jsonString;
}

void MenuManager::onFileChanged(const QString &filePath) {
    qDebug() << "MenuManager::onFileChanged called for:" << filePath;
    
    // 如果正在保存文件，则跳过重新加载（避免循环）
    if (m_isSaving) {
        qDebug() << "Skipping file change handling during save operation";
        return;
    }
    
    // 检查文件是否仍然存在（可能是删除操作）
    if (!QFile::exists(filePath)) {
        qDebug() << "File no longer exists:" << filePath;
        // 移除模型
        if (m_models.contains(filePath)) {
            m_models.remove(filePath);
        }
        // 刷新文件列表
        m_fileModel->refresh();
        // 如果是当前配置，清空当前配置
        if (m_currentConfig == filePath) {
            m_currentConfig.clear();
        }
        emit configChanged(filePath);
        return;
    }
    
    // 重新解析配置文件
    ConfigParser::ConfigData data = m_parser.parseFile(filePath);
    
    if (!data.isValid()) {
        qWarning() << "Failed to parse changed file:" << filePath;
        emit errorOccurred(QString("解析变更的配置文件失败: %1").arg(filePath));
        return;
    }
    
    // 如果已有模型，更新它；否则创建新模型
    if (m_models.contains(filePath)) {
        MenuTreeModel *model = m_models[filePath];
        model->setConfigData(data);
        qDebug() << "Updated existing model for:" << filePath;
    } else {
        MenuTreeModel *model = new MenuTreeModel(this);
        model->setConfigData(data);
        m_models[filePath] = model;
        qDebug() << "Created new model for:" << filePath;
    }
    
    // 如果是当前正在编辑的文件，发出 configLoaded 信号以刷新 UI
    if (m_currentConfig == filePath) {
        emit configLoaded(filePath);
    }
    
    // 刷新文件列表（可能文件名或其他属性变化）
    m_fileModel->refresh();
    
    // 发出配置变化信号
    emit configChanged(filePath);
}

void MenuManager::onDirectoryChanged(const QString &dirPath) {
    qDebug() << "MenuManager::onDirectoryChanged called for:" << dirPath;
    
    // 刷新文件列表
    m_fileModel->refresh();
    
    // 重新扫描目录中的文件
    m_watcher->watchDirectory(dirPath);
}

void MenuManager::saveViewState(const QString &selectedItemId, const QStringList &expandedItemIds) {
    qDebug() << "MenuManager::saveViewState called - selectedItemId:" << selectedItemId 
             << "expandedItemIds:" << expandedItemIds;
    m_selectedItemId = selectedItemId;
    m_expandedItemIds = expandedItemIds;
}

QString MenuManager::getSelectedItemId() const {
    return m_selectedItemId;
}

QStringList MenuManager::getExpandedItemIds() const {
    return m_expandedItemIds;
}
