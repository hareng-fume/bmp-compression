#include "worker.h"

#include <core.h>

//-----------------------------------------------------------------------------
Worker::Worker(QObject *ip_parent /*= nullptr*/)
    : QObject{ip_parent}
{}

//-----------------------------------------------------------------------------
void Worker::process(int i_index, QString i_filePath)
{
    const std::string filePath = i_filePath.toUtf8().constData();

    std::wstring resultingPath;
    if (is_bmp({filePath}))
        resultingPath = encode(i_filePath.toStdWString());
    else if (is_barch({filePath}))
        resultingPath = decode(i_filePath.toStdWString());

    emit finished(i_index, QString::fromStdWString(resultingPath));
}
