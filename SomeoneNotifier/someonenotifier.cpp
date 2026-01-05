#include "SomeoneNotifier.h"

SomeoneNotifier::SomeoneNotifier(QObject *parent)
    : QObject(parent)
{
}

void SomeoneNotifier::sendSomeoneHere(const QString &host, quint16 port)
{
    socket.connectToHost(host, port);

    if (socket.waitForConnected(1000)) {
        QByteArray message = QStringLiteral("有人來").toUtf8();
        socket.write(message);
        socket.flush();
        socket.waitForBytesWritten(1000);
        socket.disconnectFromHost();
    }


}

void SomeoneNotifier::show() {
    SomeoneNotifier notifier;
    notifier.sendSomeoneHere();
}
