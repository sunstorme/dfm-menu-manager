#ifndef MENUTREEMODEL_H
#define MENUTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QHash>
#include "menu_action_item.h"
#include "../core/config_parser.h"

/**
 * @brief 菜单树形模型
 * 
 * 为QML提供树形数据模型,支持菜单项的增删改查
 */
class MenuTreeModel : public QAbstractItemModel {
    Q_OBJECT
    
public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        NameLocalRole,
        IdRole,
        LevelRole,
        HasChildrenRole,
        IsEditableRole,
        IsSystemRole,
        ExecCommandRole,
        MenuTypesRole,
        SupportSuffixRole,
        PositionNumberRole
    };
    
    explicit MenuTreeModel(QObject *parent = nullptr);
    ~MenuTreeModel();
    
    // QAbstractItemModel 接口
    QModelIndex index(int row, int column, 
                     const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    // 菜单操作
    Q_INVOKABLE void addItem(const QModelIndex &parent, const QString &name);
    Q_INVOKABLE void removeItem(const QModelIndex &index);
    Q_INVOKABLE void moveItem(const QModelIndex &index, int direction);
    Q_INVOKABLE void updateItem(const QModelIndex &index, const QString &role, 
                               const QVariant &value);
    
    // 模型数据
    void setConfigData(const ConfigParser::ConfigData &data);
    MenuActionItem* getItem(const QModelIndex &index) const;
    QModelIndex getIndex(const QString &id);
    
signals:
    void errorOccurred(const QString &message);
    
private:
    MenuActionItem *m_rootItem;
    QList<MenuActionItem> m_items;
    QMap<QString, MenuActionItem*> m_itemsMap;
    
    QString generateUniqueId();
};

#endif // MENUTREEMODEL_H
