#ifndef WORKER_H
#define WORKER_H

#include <QObject>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *ip_parent = nullptr);

signals:
    void finished(int i_index, QString i_filePath);

public slots:
    void process(int i_index, QString i_filePath);
};

#endif // WORKER_H
