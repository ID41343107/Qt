#ifndef SOMEONENOTIFIER_H
#define SOMEONENOTIFIER_H

#include <QObject>
#include <QTcpSocket>

class SomeoneNotifier : public QObject
{
    Q_OBJECT
public:
    explicit SomeoneNotifier(QObject *parent = nullptr);

    void sendSomeoneHere(const QString &host = "127.0.0.1", quint16 port = 8888);

    void show();
private:
    QTcpSocket socket;
};

#endif // SOMEONENOTIFIER_H
