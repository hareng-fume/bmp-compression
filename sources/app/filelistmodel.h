#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QAbstractListModel>
#include <QByteArray>
#include <QFileInfo>
#include <QHash>
#include <QList>
#include <QModelIndex>
#include <QPointer>
#include <QSet>
#include <QVariant>

class FileListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum CustomRoles {
        NameRole = Qt::UserRole + 1,
        ExtensionRole,
        SizeRole,
        CreatedTimeRole,
        IsSupportedRole,
        IsProcessingRole
    };

    explicit FileListModel(QObject *ip_parent = nullptr);

    virtual int rowCount(const QModelIndex &i_parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &i_index, int i_role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &i_index,
                         const QVariant &i_value,
                         int i_role = NameRole) override;
    virtual QHash<int, QByteArray> roleNames() const override;

    bool canHandleItem(const QModelIndex &i_index) const;
    void addItem(const QFileInfo &i_fileInfo);
    void processItem(const QModelIndex &i_index);

    static QPointer<FileListModel> Create(const QDir &i_dir);

private slots:
    void handleWorkerFinished(int i_index, QString i_result);

private:
    QSet<size_t> m_itemHashes;
    QList<QFileInfo> m_items;
    QList<bool> m_isProcessing;
};

#endif // FILELISTMODEL_H
