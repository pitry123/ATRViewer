#pragma once
#include "QQmlAutoPropertyHelpers.h"
#include <sstream>
#include <iomanip>
#include <QUuid>
#include <QThreadPool>
#include <functional>



static std::string convertIdToStr(const long long& msb,const long long& lsb)
{
    std::stringstream msbStream;
    msbStream << std::setfill('0') << std::setw(sizeof(unsigned long long) * 2) << std::hex << static_cast<unsigned long long>(msb);
    std::string msbStr(msbStream.str());

    std::stringstream lsbStream;
    lsbStream << std::setfill('0') << std::setw(sizeof(unsigned long long) * 2) << std::hex << static_cast<unsigned long long>(lsb);
    std::string lsbStr(lsbStream.str());

    std::string idStr("{00000000-0000-0000-0000-000000000000}");
    for (unsigned int i = 1; i < 9; i++)   { idStr[i] = msbStr[i - 1];  }
    for (unsigned int i = 10; i < 14; i++) { idStr[i] = msbStr[i - 2];  }
    for (unsigned int i = 15; i < 19; i++) { idStr[i] = msbStr[i - 3];  }
    for (unsigned int i = 20; i < 24; i++) { idStr[i] = lsbStr[i - 20]; }
    for (unsigned int i = 25; i < 37; i++) { idStr[i] = lsbStr[i - 21]; }

    return idStr;
}

static bool checkIdStrFormat(const char* idStr)
{
    /*
     * Id Format = {00000000-0000-0000-0000-000000000000}
     * The zeros can be replaced by any hex char: <0-9> | <a-f> | <A-F>
     */
    return (std::strlen(idStr) == 38)
           && (idStr[0] == '{')
           && (idStr[9] == '-')
           && (idStr[14] == '-')
           && (idStr[19] == '-')
           && (idStr[24] == '-')
           && (idStr[37] == '}');
}

static void convertStrToId(const char* str, long long& msb, long long& lsb)
{
    /*
     * Removing all occurrences of '-' , '{' , '}'
     * from a string of the form {00000000-0000-0000-0000-000000000000}
     * The produced string, idHexStr is a sequential representation of the id
     * that later would be converted to two std::int_64t numbers
     */

    if(checkIdStrFormat(str) == false)
        return;

    char msbStr[17] = {0};
    char lsbStr[17] = {0};

    for (int i = 1; i < 9; i++)   { msbStr[i-1] = str[i];  }
    for (int i = 10; i < 14; i++) { msbStr[i-2] = str[i];  }
    for (int i = 15; i < 19; i++) { msbStr[i-3] = str[i];  }
    for (int i = 20; i < 24; i++) { lsbStr[i-20] = str[i]; }
    for (int i = 25; i < 37; i++) { lsbStr[i-21] = str[i]; }

    msb = static_cast<long long>(std::strtoull(msbStr, nullptr, 16));
    lsb = static_cast<long long>(std::strtoull(lsbStr, nullptr, 16));
}

template<typename T>
static QVariantList vectorToList(const QVector<T>& vec)
{
    QVariantList list;
    list.reserve(vec.size());
    for(int i = 0 ; i < vec.size() ; i++)
        list.append(QVariant::fromValue<T>(vec.at(i)));
    return list;
}

template<typename T>
static QVector<T> listToVector(const QVariantList& list)
{
    QVector<T> vec;
    vec.reserve(list.size());
    for(int i = 0 ; i < list.size() ; i++)
        vec.append(list.at(i).value<T>());
    return vec;
}


namespace QUtils
{
    static inline void runAsync(const std::function<void()>& func)
    {
        class runner :public QRunnable
        {
        private:
            std::function<void()> m_func;

        public:
            runner(const std::function<void()>& func) :
                m_func(func)
            {
            }

            void run() override
            {
                m_func();
            }
        };

        QThreadPool::globalInstance()->start(new runner(func));
    }

    static inline void runAsyncWait(const std::function<void()>& func)
    {
        class runner :public QRunnable
        {
        private:
            std::function<void()> m_func;

        public:
            runner(const std::function<void()>& func) :
                m_func(func)
            {
            }

            void run() override
            {
                m_func();
            }
        };

        runner* instance = new runner(func);
        while (QThreadPool::globalInstance()->tryStart(instance) == false)
            std::this_thread::yield();
    }
}
