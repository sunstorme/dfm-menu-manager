#include "menu_tree_model.h"
#include <QDateTime>

MenuTreeModel::MenuTreeModel(QObject *parent)
    : QAbstractItemModel(parent) {
    m_rootItem = new MenuActionItem();
    m_rootItem->isRoot = true;
    m_rootItem->level = 0;
    m_rootItem->id = "root";
}

MenuTreeModel::~MenuTreeModel() {
    delete m_rootItem;
}

QModelIndex MenuTreeModel::index(int row, int column, 
                                 const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }
    
    MenuActionItem *parentItem = parent.isValid() 
        ? static_cast<MenuActionItem*>(parent.internalPointer())
        : m_rootItem;
    
    if (!parentItem || row >= parentItem->childActions.size()) {
        return QModelIndex();
    }
    
    QString childId = parentItem->childActions[row];
    MenuActionItem *childItem = m_itemsMap.value(childId);
    
    if (!childItem) {
        return QModelIndex();
    }
    
    return createIndex(row, column, childItem);
}

QModelIndex MenuTreeModel::parent(const QModelIndex &child) const {
    if (!child.isValid()) {
        return QModelIndex();
    }
    
    MenuActionItem *childItem = static_cast<MenuActionItem*>(child.internalPointer());
    if (!childItem || childItem->level <= 1) {
        return QModelIndex();
    }
    
    // 查找父项
    for (auto &item : m_items) {
        if (item.childActions.contains(childItem->id)) {
            int row = item.childActions.indexOf(childItem->id);
            return createIndex(row, 0, &item);
        }
    }
    
    return QModelIndex();
}

int MenuTreeModel::rowCount(const QModelIndex &parent) const {
    MenuActionItem *parentItem = parent.isValid()
        ? static_cast<MenuActionItem*>(parent.internalPointer())
        : m_rootItem;
    
    if (!parentItem) {
        return 0;
    }
    
    return parentItem->childActions.size();
}

int MenuTreeModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 1;
}

QVariant MenuTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    
    MenuActionItem *item = static_cast<MenuActionItem*>(index.internalPointer());
    if (!item) {
        return QVariant();
    }
    
    switch (role) {
    case NameRole:
        return item->name;
    case NameLocalRole:
        return item->nameLocal.isEmpty() ? item->name : item->nameLocal;
    case IdRole:
        return item->id;
    case LevelRole:
        return item->level;
    case HasChildrenRole:
        return !item->childActions.isEmpty();
    case IsEditableRole:
        return !item->isSystem;
    case IsSystemRole:
        return item->isSystem;
    case ExecCommandRole:
        return item->execCommand;
    case MenuTypesRole:
        return item->menuTypes;
    case SupportSuffixRole:
        return item->supportSuffix;
    case PositionNumberRole:
        return item->positionNumber;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> MenuTreeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[NameLocalRole] = "nameLocal";
    roles[IdRole] = "id";
    roles[LevelRole] = "level";
    roles[HasChildrenRole] = "hasChildren";
    roles[IsEditableRole] = "isEditable";
    roles[IsSystemRole] = "isSystem";
    roles[ExecCommandRole] = "execCommand";
    roles[MenuTypesRole] = "menuTypes";
    roles[SupportSuffixRole] = "supportSuffix";
    roles[PositionNumberRole] = "positionNumber";
    return roles;
}

void MenuTreeModel::addItem(const QModelIndex &parent, const QString &name) {
    if (!parent.isValid()) {
        emit errorOccurred("无效的父项");
        return;
    }
    
    MenuActionItem *parentItem = static_cast<MenuActionItem*>(parent.internalPointer());
    
    // 检查层级限制
    if (parentItem->level >= 3) {
        emit errorOccurred("最多支持3级菜单");
        return;
    }
    
    beginInsertRows(parent, parentItem->childActions.size(), 
                   parentItem->childActions.size());
    
    // 创建新菜单项
    MenuActionItem newItem;
    newItem.id = generateUniqueId();
    newItem.name = name;
    newItem.nameLocal = name;
    newItem.level = parentItem->level + 1;
    newItem.positionNumber = parentItem->childActions.size() + 1;
    newItem.configFile = parentItem->configFile;
    newItem.isSystem = parentItem->isSystem;
    
    m_items.append(newItem);
    m_itemsMap[newItem.id] = &m_items.last();
    parentItem->childActions.append(newItem.id);
    
    endInsertRows();
}

void MenuTreeModel::removeItem(const QModelIndex &index) {
    if (!index.isValid()) {
        return;
    }
    
    MenuActionItem *item = static_cast<MenuActionItem*>(index.internalPointer());
    if (item->isSystem) {
        emit errorOccurred("系统配置不能删除");
        return;
    }
    
    QModelIndex parent = index.parent();
    MenuActionItem *parentItem = parent.isValid()
        ? static_cast<MenuActionItem*>(parent.internalPointer())
        : m_rootItem;
    
    if (!parentItem) {
        return;
    }
    
    int row = index.row();
    
    beginRemoveRows(parent, row, row);
    
    parentItem->childActions.removeAt(row);
    m_itemsMap.remove(item->id);
    m_items.removeAll(*item);
    
    endRemoveRows();
}

void MenuTreeModel::moveItem(const QModelIndex &index, int direction) {
    if (!index.isValid()) {
        return;
    }
    
    QModelIndex parent = index.parent();
    MenuActionItem *parentItem = parent.isValid()
        ? static_cast<MenuActionItem*>(parent.internalPointer())
        : m_rootItem;
    
    if (!parentItem) {
        return;
    }
    
    int row = index.row();
    int newRow = row + direction;
    
    if (newRow < 0 || newRow >= parentItem->childActions.size()) {
        return;
    }
    
    // 交换位置
    beginMoveRows(parent, row, row, parent, direction > 0 ? newRow + 1 : newRow);
    
    parentItem->childActions.swapItemsAt(row, newRow);
    
    endMoveRows();
}

void MenuTreeModel::updateItem(const QModelIndex &index, const QString &role, 
                               const QVariant &value) {
    if (!index.isValid()) {
        return;
    }
    
    MenuActionItem *item = static_cast<MenuActionItem*>(index.internalPointer());
    if (!item) {
        return;
    }
    
    if (role == "name") {
        item->name = value.toString();
    } else if (role == "nameLocal") {
        item->nameLocal = value.toString();
    } else if (role == "execCommand") {
        item->execCommand = value.toString();
    } else if (role == "menuTypes") {
        item->menuTypes = value.toStringList();
    } else if (role == "supportSuffix") {
        item->supportSuffix = value.toStringList();
    } else if (role == "positionNumber") {
        item->positionNumber = value.toInt();
    }
    
    emit dataChanged(index, index);
}

void MenuTreeModel::setConfigData(const ConfigParser::ConfigData &data) {
    beginResetModel();
    
    m_items = data.actions;
    m_itemsMap.clear();
    for (auto &item : m_items) {
        m_itemsMap[item.id] = &item;
    }
    
    if (data.actionMap.contains("root")) {
        m_rootItem = data.actionMap["root"];
    }
    
    endResetModel();
}

MenuActionItem* MenuTreeModel::getItem(const QModelIndex &index) const {
    if (!index.isValid()) {
        return nullptr;
    }
    
    return static_cast<MenuActionItem*>(index.internalPointer());
}

QModelIndex MenuTreeModel::getIndex(const QString &id) {
    if (!m_itemsMap.contains(id)) {
        return QModelIndex();
    }
    
    MenuActionItem *item = m_itemsMap[id];
    if (!item) {
        return QModelIndex();
    }
    
    // 查找父项
    for (auto &parentItem : m_items) {
        if (parentItem.childActions.contains(id)) {
            int row = parentItem.childActions.indexOf(id);
            return createIndex(row, 0, item);
        }
    }
    
    // 如果是根项的子项
    if (m_rootItem && m_rootItem->childActions.contains(id)) {
        int row = m_rootItem->childActions.indexOf(id);
        return createIndex(row, 0, item);
    }
    
    return QModelIndex();
}

QString MenuTreeModel::generateUniqueId() {
    static int counter = 0;
    return QString("action_%1_%2").arg(QDateTime::currentMSecsSinceEpoch())
                                     .arg(counter++);
}
