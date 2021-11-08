#include "QDatabaseConnector.h"
#include <QQmlEngine>

void QDatabaseConnector::onQbjectInvoke(const std::function<void ()> &func)
{
    func();
}

bool QDatabaseConnector::emitRequierd()
{
    return (this->thread() != QThread::currentThread());
}

void QDatabaseConnector::emitASync(const std::function<void ()> &func)
{
    if (emitRequierd() == true)
    {
        emit qobjectInvoke(func);
    }
    else
    {
        func();
    }
}

void QDatabaseConnector::emitASyncBlock(const std::function<void ()> &func)
{
    std::mutex mutex;
    bool completed = false;
    std::condition_variable waitHandle;

    emitASync([&]()
    {
        func();
        std::unique_lock<std::mutex> locker(mutex);
        completed = true;
        locker.unlock();
        waitHandle.notify_one();
    });

    std::unique_lock<std::mutex> locker(mutex);
    waitHandle.wait(locker, [&]()
    {
        return completed;
    });
}

QDatabaseConnector::QDatabaseConnector(const Utils::Context &context, QObject *parent) :
    QDatabaseConnectorBase(context, parent)
{
    static std::once_flag REGISTER_ONCE_FLAG;
    std::call_once(REGISTER_ONCE_FLAG, []()
    {
        qRegisterMetaType<std::function<void()>>();
    });

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    connect(this, &QDatabaseConnector::qobjectInvoke, this, &QDatabaseConnector::onQbjectInvoke);
}

QDatabaseConnector::QDatabaseConnector(QObject *parent) :
    QDatabaseConnector(nullptr, parent)
{
}

QDatabaseConnector::~QDatabaseConnector()
{
    disconnect(this, &QDatabaseConnector::qobjectInvoke, this, &QDatabaseConnector::onQbjectInvoke);
}
