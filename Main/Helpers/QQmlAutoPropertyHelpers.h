#pragma once
#include "Helpers/Constants.h"
#include "Helpers/QQmlHelpersCommon.h"

#include <QObject>
#include <QQmlListProperty>

#include <mutex>
#include <atomic>
#include <functional>

#include <Utils.hpp>

#ifdef TIMER_PROPERTIES

class PropertyUpdateTimer : public Utils::Timer
{
private:
    PropertyUpdateTimer()
    {
    }

public:
    static PropertyUpdateTimer& Instance()
    {
        static PropertyUpdateTimer INSTANCE;
        return INSTANCE;
    }
};

class PropertyUpdateTimerToken
{
private:
    Utils::TimerToken m_token;

    PropertyUpdateTimerToken(const PropertyUpdateTimerToken& other) = delete;
    PropertyUpdateTimerToken& operator=(const PropertyUpdateTimerToken& other);

public:
    PropertyUpdateTimerToken(Utils::TimerToken token) :
        m_token(token)
    {
    }

    PropertyUpdateTimerToken() :
        PropertyUpdateTimerToken(Utils::TimerTokenUndefined)
    {
    }

    PropertyUpdateTimerToken(PropertyUpdateTimerToken&& other) :
        m_token(other.m_token)
    {
        other.m_token = Utils::TimerTokenUndefined;
    }

    inline bool Undefined()
    {
        return (m_token == Utils::TimerTokenUndefined);
    }

    ~PropertyUpdateTimerToken()
    {
        if (Undefined() == false)
            PropertyUpdateTimer::Instance().Elapsed() -= m_token;
    }

    PropertyUpdateTimerToken& operator=(PropertyUpdateTimerToken&& other)
    {
        m_token = other.m_token;
        other.m_token = Utils::TimerTokenUndefined;
        return *this;
    }
};

#endif

// NOTE : individual macros for getter, setter, notifier, and member

#define QML_AUTO_GETTER(type, name) \
    CheapestType<type>::type_def MAKE_GETTER_NAME(name) (void) const { \
        std::lock_guard<std::mutex> locker(m_##name##_mutex); \
        return m_##name; \
    }

#ifdef TIMER_PROPERTIES
#define QML_AUTO_SETTER(type, name, prefix) \
    bool prefix##name (CheapestType<type>::type_def name) { \
    std::unique_lock<std::mutex> locker(m_##name##_mutex); \
    if (m_##name != name) { \
            m_##name = name; \
            m_is_##name##_dirty = true; \
            if (m_##name##_emit_timer_token.Undefined() == true) \
            { \
                m_##name##_emit_timer_token = PropertyUpdateTimer::Instance().Elapsed() += [this]() \
                { \
                    std::unique_lock<std::mutex> locker(m_##name##_mutex); \
                    if (m_is_##name##_dirty == true) \
                    { \
                        locker.unlock(); \
                        emit name##Changed (m_##name); \
                        m_is_##name##_dirty = false; \
                    } \
                }; \
            } \
            return true; \
        } \
        else { \
            return false; \
        } \
    }
#else
#define QML_AUTO_SETTER(type, name, prefix) \
    bool prefix##name (CheapestType<type>::type_def name) { \
        std::unique_lock<std::mutex> locker(m_##name##_mutex); \
        if (m_##name != name) { \
            m_##name = name; \
            locker.unlock(); \
            emit name##Changed (name); \
            return true; \
        } \
        else { \
            return false; \
        } \
    }
#endif

#define QML_AUTO_NOTIFIER(type, name) \
    void name##Changed(type name);

#ifdef TIMER_PROPERTIES
#define QML_AUTO_MEMBER(type, name) \
    mutable std::mutex m_##name##_mutex; \
    type m_##name; \
    std::atomic<bool> m_is_##name##_dirty = {false}; \
    PropertyUpdateTimerToken m_##name##_emit_timer_token;
#else
#define QML_AUTO_MEMBER(type, name) \
    mutable std::mutex m_##name##_mutex; \
    type m_##name;
#endif

// NOTE : actual auto-property helpers

#define QML_WRITABLE_AUTO_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ MAKE_GETTER_NAME(name) WRITE set_##name NOTIFY name##Changed) \
    private: \
        QML_AUTO_MEMBER (type, name) \
    public: \
        QML_AUTO_GETTER (type, name) \
        QML_AUTO_SETTER (type, name, set_) \
    Q_SIGNALS: \
        QML_AUTO_NOTIFIER (type, name) \
    private:

#define QML_READONLY_AUTO_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ MAKE_GETTER_NAME(name) NOTIFY name##Changed) \
    private: \
        QML_AUTO_MEMBER (type, name) \
    public: \
        QML_AUTO_GETTER (type, name) \
        QML_AUTO_SETTER (type, name, update_) \
    Q_SIGNALS: \
        QML_AUTO_NOTIFIER (type, name) \
    private:

#define QML_CONSTANT_AUTO_PROPERTY(type, name) \
    protected: \
        Q_PROPERTY (type name READ MAKE_GETTER_NAME(name) CONSTANT) \
    private: \
        QML_AUTO_MEMBER (type, name) \
    public: \
        QML_AUTO_GETTER (type, name) \
    private:

#define QML_LIST_PROPERTY(host, type) \
    Q_PROPERTY(QQmlListProperty<type> items READ items NOTIFY itemsChanged) \
Q_SIGNALS: \
    void itemsChanged(); \
protected: \
    static void AppendFunction(QQmlListProperty<type>* list, type* item) \
    { \
        host* instance = qobject_cast<host*>(list->object); \
        instance->appendItem(item); \
    } \
    static int CountFunction(QQmlListProperty<type>* list) \
    { \
        host* instance = qobject_cast<host*>(list->object); \
        return instance->count(); \
    } \
    static type* AtFunction(QQmlListProperty<type>* list, int index) \
    { \
        host* instance = qobject_cast<host*>(list->object); \
        return instance->at(index); \
    } \
    static void ClearFunction(QQmlListProperty<type>* list) \
    { \
        host* instance = qobject_cast<host*>(list->object); \
        instance->clear(); \
    } \
    mutable std::mutex m_generated_types_list_mutex; \
    QList<type*> m_generated_types_list; \
    void appendItemUnsafe(type* item) \
    { \
        std::unique_lock<std::mutex> locker(m_generated_types_list_mutex); \
        m_generated_types_list.append(item); \
        locker.unlock(); \
        emit itemsChanged(); \
    } \
    void insertItemUnsafe(int i, type* item) \
    { \
        std::unique_lock<std::mutex> locker(m_generated_types_list_mutex); \
        m_generated_types_list.insert(i, item); \
        locker.unlock(); \
        emit itemsChanged(); \
    } \
    int removeItemUnsafe(type* item) \
    { \
        std::unique_lock<std::mutex> locker(m_generated_types_list_mutex); \
        int retval = m_generated_types_list.removeAll(item); \
        locker.unlock(); \
        if (retval > 0) \
            emit itemsChanged(); \
        return retval; \
    } \
    void clearUnsafe() \
    { \
        std::unique_lock<std::mutex> locker(m_generated_types_list_mutex); \
        if (m_generated_types_list.count() == 0) \
            return; \
        m_generated_types_list.clear(); \
        locker.unlock(); \
        emit itemsChanged(); \
    } \
public: \
    int count() \
    { \
        std::lock_guard<std::mutex> locker(m_generated_types_list_mutex); \
        return m_generated_types_list.count(); \
    } \
    type* at(int index) \
    { \
        std::lock_guard<std::mutex> locker(m_generated_types_list_mutex); \
        if (index < 0 || m_generated_types_list.size() <= index) \
            return nullptr; \
        return m_generated_types_list.at(index); \
    } \
    virtual void appendItem(type* item) \
    { \
        appendItemUnsafe(item); \
    } \
    virtual void insertItem(int i, type* item) \
    { \
        insertItemUnsafe(i, item); \
    } \
    virtual int removeItem(type* item) \
    { \
        return removeItemUnsafe(item); \
    } \
    virtual void clear() \
    { \
        clearUnsafe(); \
    } \
    QQmlListProperty<type> items() \
    { \
        return QQmlListProperty<type>( \
                    this, \
                    nullptr, \
                    &host::AppendFunction, \
                    &host::CountFunction, \
                    &host::AtFunction, \
                    &host::ClearFunction); \
    } \
private:

#define QML_LIST_CLASS(name, type) \
class name : public QObject \
{ \
    Q_OBJECT \
    QML_LIST_PROPERTY(name, type) \
public: \
    explicit name(QObject* parent = nullptr) : \
        QObject(parent) \
    { \
        if (parent != nullptr) \
            moveToThread(parent->thread()); \
    } \
};
