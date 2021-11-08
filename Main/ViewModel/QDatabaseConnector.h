#pragma once
#include <QObject>
#include <QThread>

#include <utils/ref_count_ptr.hpp>
#include <Database.hpp>

#include <functional>
#include <mutex>
#include <condition_variable>

Q_DECLARE_METATYPE(std::function<void()>)

template <typename T>
class QDatabaseConnectorBase : public T
{
private:
    Database::Subscriber m_subscriber;

protected:
    virtual Database::SubscriptionParams Subscribe(const Database::Row& row, const std::function<void(const Database::RowData&)>& func)
    {
        return m_subscriber.Subscribe(row, func);
    }

    virtual Database::SubscriptionParams Subscribe(const Database::Table& table, const Database::AnyKey& rowKey, const std::function<void(const Database::RowData&)>& func)
    {
        return m_subscriber.Subscribe(table, rowKey, func);
    }

    virtual Database::SubscriptionParams Subscribe(const Database::DataSet& dataset, const Database::AnyKey& tableKey, const Database::AnyKey& rowKey, const std::function<void(const Database::RowData&)>& func)
    {
        return m_subscriber.Subscribe(dataset, tableKey, rowKey, func);
    }

    virtual bool Unsubscribe(const Database::Row& row, Database::SubscriptionToken token)
    {
        return m_subscriber.Unsubscribe(row, token);
    }

    virtual bool Unsubscribe(const Database::Table& table, const Database::AnyKey& rowKey, Database::SubscriptionToken token)
    {
        return m_subscriber.Unsubscribe(table, rowKey, token);
    }

    virtual bool Unsubscribe(const Database::DataSet& dataset, const Database::AnyKey& tableKey, const Database::AnyKey& rowKey, Database::SubscriptionToken token)
    {
        return m_subscriber.Unsubscribe(dataset, tableKey, rowKey, token);
    }

    virtual Utils::Context Context()
    {
        return m_subscriber.Context();
    }

    Utils::TimerRegistrationParams RegisterTimer(double interval, const std::function<void()>& func, unsigned int invocationCount = 0)
    {
        return Context().RegisterTimer(interval, func, invocationCount);
    }

    bool UnregisterTimer(Utils::TimerToken token)
    {
        return Context().UnregisterTimer(token);
    }

    bool UnregisterTimer(const Utils::TimerRegistrationParams registrationParams)
    {
        return Context().UnregisterTimer(registrationParams);
    }

public:
    QDatabaseConnectorBase(const Utils::Context& context, QObject* parent = nullptr) :
        T(parent),
        m_subscriber(utils::make_ref_count_ptr<Database::Dispatcher>(context))
    {        
    }

    explicit QDatabaseConnectorBase(QObject* parent = nullptr) :
        QDatabaseConnectorBase(nullptr, parent)
    {
    }

    virtual ~QDatabaseConnectorBase() override = default;

    virtual void Init()
    {
        // Do Nothnig...
    }

    virtual void Start()
    {
        // Do Nothnig...
    }

    virtual void Stop()
    {
        // Do Nothnig...
    }
};

class QDatabaseConnector : public QDatabaseConnectorBase<QObject>
{
    Q_OBJECT

signals:
    void qobjectInvoke(const std::function<void()>& func);

private slots:
    void onQbjectInvoke(const std::function<void()>& func);

protected:
    bool emitRequierd();
    void emitASync(const std::function<void()>& func);
    void emitASyncBlock(const std::function<void()>& func);

public:
    QDatabaseConnector(const Utils::Context& context, QObject* parent = nullptr);
    explicit QDatabaseConnector(QObject* parent = nullptr);
    virtual ~QDatabaseConnector() override;
};
