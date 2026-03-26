#include "menu_manager.h"
#include "file_watcher.h"
#include "../utils/file_utils.h"
#include <QDebug>
#include <QFile>

MenuManager::MenuManager(QObject *parent)
    : QObject(parent)
    , m_watcher(new FileWatcher(this))
    , m_fileModel(new MenuFileModel(this))
{
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
    
    // TODO: 从模型获取配置数据并保存
    emit configSaved(filePath);
    return true;
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
    if (!m_models.contains(configFile)) {
        // 解析配置文件
        ConfigParser::ConfigData data = m_parser.parseFile(configFile);
        
        if (!data.isValid()) {
            emit errorOccurred("解析配置文件失败");
            return nullptr;
        }
        
        // 创建模型
        MenuTreeModel *model = new MenuTreeModel(this);
        model->setConfigData(data);
        m_models[configFile] = model;
    }
    
    return m_models[configFile];
}

void MenuManager::setCurrentConfig(const QString &configFile) {
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
