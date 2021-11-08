#include "VideoStreamAllocator.h"
#include "ViewModel/QVideo.h"
#include <Core.hpp>
#include <iostream>

#define EXIT_TIME_OUT 10000
#define TIMER_TIME_OUT 10000
#define TERMINATED_PERIOD_TIMEOUT 5000

Logging::Logger videoLogger = Core::Framework::CreateLogger("VideoStreamAllocator", Logging::Severity::INFO);

void VideoStreamAllocator::onOpenStream(const QString& name, const AtrViewer::QVideoChannel* videoChannel)
{
    createPublisher(name, videoChannel);
}

void VideoStreamAllocator::onCloseStream(const QString& name)
{
    if (m_aPublishers.find(name) == m_aPublishers.end())
    {
        LOG_ERROR(videoLogger) << "failed to close a stream publisher";
        return;
    }
    QPointer<QProcess> publisher = m_aPublishers[name].publisher;
    publisher->write("suzi");
    Publisher tmpPub;
    tmpPub.videoChannel = nullptr;
    tmpPub.publisher = publisher;
    tmpPub.time = std::chrono::high_resolution_clock::now();
    m_aTemporaryPublishers[name] = tmpPub;
    m_aPublishers.remove(name);
}

VideoStreamAllocator::VideoStreamAllocator(QObject* parent) : QThread(parent)
{
    moveToThread(this);
    connect(this, &VideoStreamAllocator::openStream, this, &VideoStreamAllocator::onOpenStream);
    connect(this, &VideoStreamAllocator::closeStream, this, &VideoStreamAllocator::onCloseStream);
    start();
}

VideoStreamAllocator::~VideoStreamAllocator()
{
    exit();
    disconnect(this, &VideoStreamAllocator::openStream, this, &VideoStreamAllocator::onOpenStream);
    disconnect(this, &VideoStreamAllocator::closeStream, this, &VideoStreamAllocator::onCloseStream);
    wait(EXIT_TIME_OUT);
}

void VideoStreamAllocator::run()
{
    int id = startTimer(TIMER_TIME_OUT);
    if (id == 0 )
        LOG_ERROR(videoLogger) << "Failed to start timer";
    exec();
    if (id != 0)
        killTimer(id);
}

void VideoStreamAllocator::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    for (auto it = m_aTemporaryPublishers.begin() ; it != m_aTemporaryPublishers.end() ; )
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now() - it.value().time).count() > TERMINATED_PERIOD_TIMEOUT)
        {
            if (it.value().publisher->state() == QProcess::ProcessState::Running)
            {
                it.value().publisher->kill();
            }
            it.value().publisher->deleteLater();
            m_aTemporaryPublishers.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    for (auto it = m_aPublishers.begin() ; it != m_aPublishers.end() ; )
    {
        if (it.value().publisher->state() == QProcess::ProcessState::NotRunning)
        {
            QString name = it.key();
            const AtrViewer::QVideoChannel* videoChannel = it.value().videoChannel;
            it.value().publisher->kill();
            it.value().publisher->deleteLater();
            m_aPublishers.erase(it++);

            createPublisher(name, videoChannel);
        }
        else
        {
            ++it;
        }
    }
}

void VideoStreamAllocator::createPublisher(const QString& name, const AtrViewer::QVideoChannel* videoChannel)
{
    if (m_aPublishers.find(name) != m_aPublishers.end())
        return;

    if (videoChannel == nullptr)
    {
        LOG_FATAL(videoLogger) << "VideoStreamAllocator::createPublisher argument videoChannel is null";
        return;
    }

    if (m_aTemporaryPublishers.contains(name)) {
        m_aTemporaryPublishers[name].publisher->waitForFinished(1000);
        m_aTemporaryPublishers[name].publisher->kill();
        m_aTemporaryPublishers[name].publisher.clear();
        m_aTemporaryPublishers.remove(name);
    }

    QPointer<QProcess> publisher = QPointer<QProcess>(new QProcess());
    QStringList argList;
    QString argName;
    argName.append("-name:");
    argName.append(name);
    argList.append(argName);

    std::istringstream argsIss(videoChannel->get_url().toStdString());
    std::vector<std::string> argsVector((std::istream_iterator<std::string>(argsIss)),
                                        (std::istream_iterator<std::string>()));

    bool start_collect = false;
    bool end_collect = true;
    std::string collector = "";
    for (std::string arg : argsVector)
    {
        if(arg.at(0) == '"' || start_collect == true)
        {
            start_collect = true;
            if(arg.at(0) == '"')
            {
                collector += arg.substr(1,arg.length() - 1) + " ";
            }

            else if(arg.at(arg.length() - 1) == '"')
            {
                collector += arg.substr(0,arg.length() - 1);
                start_collect = false;
                argList.append(collector.c_str());
                collector = "";
            }
            else
            {
                collector += arg + " ";
            }
        }
        else
        {
            argList.append(arg.c_str());
        }
    }

    for(auto ss: argList)
    {
        std::cout << ss.toStdString()+" \n";
    }

    publisher->setArguments(argList);
#ifdef _WIN32
    publisher->setProgram("VideoPublisher.exe");
#else
    publisher->setProgram("VideoPublisher");
#endif
    publisher->start();
    m_aPublishers[name].publisher = publisher;
    m_aPublishers[name].videoChannel = videoChannel;
}

