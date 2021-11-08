#pragma once
#include <QObject>
#include <functional>

class QQmlReload : public QObject
{
    Q_OBJECT

private:
    static std::function<void()> m_reloadFunc;

public:
    static void SetReloadFunc(const std::function<void()>& func)
    {
        m_reloadFunc = func;
    }

    Q_INVOKABLE void reload()
    {
        if (m_reloadFunc)
        {
            m_reloadFunc();
        }
    }
};
