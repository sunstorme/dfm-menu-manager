#ifndef MENUFILEMODEL_H
#define MENUFILEMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class MenuFileModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(bool showSystemOnly READ showSystemOnly WRITE setShowSystemOnly NOTIFY showSystemOnlyChanged)
    Q_PROPERTY(QString searchFilter READ searchFilter WRITE setSearchFilter NOTIFY searchFilterChanged)
    
public:
    enum Roles {
        FileNameRole = Qt::UserRole + 1,
        FilePathRole,
        IsSystemRole,
        IsModifiedRole,
        CommentRole
    };
    
    explicit MenuFileModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    Q_INVOKABLE void refresh();
    void setSearchFilter(const QString &filter);
    QString searchFilter() const { return m_searchFilter; }
    Q_INVOKABLE void createFile(const QString &name);
    Q_INVOKABLE void deleteFile(const QString &path);
    Q_INVOKABLE void renameFile(const QString &path, const QString &newName);
    Q_INVOKABLE QString copyFile(const QString &sourcePath, bool toSystem = false);
    
    bool showSystemOnly() const { return m_showSystemOnly; }
    void setShowSystemOnly(bool show) { 
        if (m_showSystemOnly != show) {
            m_showSystemOnly = show;
            emit showSystemOnlyChanged();
            refresh();
        }
    }
    
signals:
    void showSystemOnlyChanged();
    void searchFilterChanged();
    
private:
    void applySearchFilter();
    
    struct FileInfo {
        QString name;
        QString path;
        bool isSystem;
        bool isModified;
        QString comment;
    };
    
    QList<FileInfo> m_files;
    QList<FileInfo> m_allFiles;  // 保存所有文件，用于搜索过滤
    bool m_showSystemOnly;
    QString m_searchFilter;
};

#endif // MENUFILEMODEL_H
