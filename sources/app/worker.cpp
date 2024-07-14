#include "worker.h"

#include <core.h>

//-----------------------------------------------------------------------------
Worker::Worker(QObject *ip_parent /*= nullptr*/)
    : QObject{ip_parent}
{}

//-----------------------------------------------------------------------------
void Worker::process(int i_index, QString i_filePath)
{
    const std::filesystem::path filePath(i_filePath.toStdWString());

    std::filesystem::path resultingPath;
    if (is_bmp(filePath))
        resultingPath = encode(filePath);
    else if (is_barch(filePath))
        resultingPath = decode(filePath);

    emit finished(i_index, QString::fromStdWString(resultingPath.wstring()));
}
