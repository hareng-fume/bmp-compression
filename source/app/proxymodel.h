#ifndef PROXYMODEL_H
#define PROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QString>

class FileListModel;

class ProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterPattern READ filterPattern WRITE setFilterPattern NOTIFY
                   filterPatternChanged FINAL)
public:
    explicit ProxyModel(QObject *ip_parent = nullptr);

    QString filterPattern() const;
    void setFilterPattern(const QString &i_pattern);

public slots:
    void handleItemClick(int i_index) const;
    bool canHandleItem(int i_index) const;

protected:
    virtual bool filterAcceptsRow(int i_sourceRow, const QModelIndex &i_sourceParent) const override;

signals:
    void filterPatternChanged();

private:
    QString m_filterPattern;
};

#endif // PROXYMODEL_H
