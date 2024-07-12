#include "filelistmodel.h"
#include "worker.h"

#ifdef DEBUG_BUILD
#include <QDebug>
#endif

#include <QDirIterator>
#include <QModelIndex>
#include <QThread>

namespace {

//-----------------------------------------------------------------------------
static constexpr char _BMP[] = "bmp";
static constexpr char _BARCH[] = "barch";

//-----------------------------------------------------------------------------
bool _IsBmp(const QString &i_suffix)
{
    return i_suffix == _BMP;
}

//-----------------------------------------------------------------------------
bool _IsBarch(const QString &i_suffix)
{
    return i_suffix == _BARCH;
}

} // namespace

//-----------------------------------------------------------------------------
FileListModel::FileListModel(QObject *ip_parent /*= nullptr*/)
    : QAbstractListModel{ip_parent}
{}

//-----------------------------------------------------------------------------
int FileListModel::rowCount(const QModelIndex &i_parent /*= QModelIndex()*/) const
{
    Q_UNUSED(i_parent);
    return m_items.count();
}

//-----------------------------------------------------------------------------
QVariant FileListModel::data(const QModelIndex &i_index, int i_role /*= Qt::DisplayRole*/) const
{
    if (!i_index.isValid() || i_index.row() >= m_items.size())
        return QVariant();

    const auto &item = m_items[i_index.row()];
    if (i_role == NameRole) {
        return item.fileName();
    } else if (i_role == ExtensionRole) {
        return item.suffix();
    } else if (i_role == SizeRole) {
        const auto bytes = item.size();
        const auto k_bytes = bytes >> 10;
        return QString("%1 KB (%2 B)").arg(k_bytes).arg(bytes);
    } else if (i_role == CreatedTimeRole) {
        return item.birthTime().toString();
    } else if (i_role == IsSupportedRole) {
        return _IsBmp(item.suffix()) || _IsBarch(item.suffix());
    } else if (i_role == IsProcessingRole) {
        if (m_isProcessing[i_index.row()]) {
            if (_IsBmp(item.suffix())) {
                return QString("compressing...");
            } else if (_IsBarch(item.suffix())) {
                return QString("decompressing...");
            }
        }
        return QString();
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
bool FileListModel::setData(const QModelIndex &i_index,
                            const QVariant &i_value,
                            int i_role /*= NameRole*/)
{
    if (!i_index.isValid() || i_index.row() >= m_items.size())
        return false;

    if (i_role == IsProcessingRole) {
        m_isProcessing[i_index.row()] = i_value.toBool();
        emit dataChanged(i_index, i_index, {i_role});
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
QHash<int, QByteArray> FileListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "file_name";
    roles[ExtensionRole] = "file_extension";
    roles[SizeRole] = "file_size";
    roles[CreatedTimeRole] = "created_time";
    roles[IsSupportedRole] = "is_supported";
    roles[IsProcessingRole] = "is_processing";
    return roles;
}

//-----------------------------------------------------------------------------
void FileListModel::addItem(const QFileInfo &i_fileInfo)
{
    const size_t fileHash = std::hash<std::wstring>{}(i_fileInfo.absoluteFilePath().toStdWString());
    if (m_itemHashes.contains(fileHash))
        return;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_items.push_back(i_fileInfo);
    m_itemHashes.insert(fileHash);
    m_isProcessing.push_back(false);
    endInsertRows();
}

//-----------------------------------------------------------------------------
void FileListModel::processItem(const QModelIndex &i_index)
{
    if (m_isProcessing[i_index.row()])
        return;

    setData(i_index, true, IsProcessingRole);
#ifdef DEBUG_BUILD
    qDebug() << "is_processing[" << i_index.row() << "]: true";
#endif

    QThread *p_thread = new QThread;
    Worker *p_worker = new Worker;

    p_worker->moveToThread(p_thread);
    connect(p_thread, &QThread::started, p_worker, [p_worker, i = i_index.row(), this]() {
        p_worker->process(i, this->m_items[i].filePath());
    });
    connect(p_worker, &Worker::finished, this, &FileListModel::handleWorkerFinished);
    connect(p_worker, &Worker::finished, p_thread, &QThread::quit);
    connect(p_thread, &QThread::finished, p_worker, &Worker::deleteLater);
    connect(p_thread, &QThread::finished, p_thread, &QThread::deleteLater);

    p_thread->start();
}

//-----------------------------------------------------------------------------
QPointer<FileListModel> FileListModel::Create(const QDir &i_dir)
{
    QDirIterator dirIt(i_dir.absolutePath(),
                       QStringList() << "*.bmp"
                                     << "*.png"
                                     << "*.barch",
                       QDir::Files,
                       QDirIterator::Subdirectories);

    auto p_model = new FileListModel;
    while (dirIt.hasNext()) {
        p_model->addItem(dirIt.nextFileInfo());
    }

    return p_model;
}

//-----------------------------------------------------------------------------
void FileListModel::handleWorkerFinished(int i_index, QString i_result)
{
    addItem(QFileInfo{i_result});
    setData(createIndex(i_index, 0), false, IsProcessingRole);
#ifdef DEBUG_BUILD
    qDebug() << "is_processing[" << i_index << "]: false";
#endif
}
