#include "QResources.h"
#include <QQmlEngine>
#include <QEvent>
#include <QCoreApplication>

VG::QResource::QResource(
        const QResourceIdentifier &identifier,
        QObject *parent) :
    QObject(nullptr),
    m_identifier(identifier),
    m_children(new QResourceList(nullptr)),
    m_owner(nullptr)
{
    // QResources are ALWAYS managed by CPP
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(this, SIGNAL(asyncSetParent(QObject*)), this, SLOT(onAsyncSetParent(QObject*)));

    if (parent != nullptr)
    {
        if (parent->thread() == QThread::currentThread())
        {
            setParent(parent);
            m_children->setParent(this);
        }
        else
        {
            if (parent->thread() != nullptr)
            {                
                moveToThread(parent->thread());
                m_children->moveToThread(parent->thread());

                emit asyncSetParent(parent);
            }
        }
    }
    else
    {
        if (this->thread() == nullptr)
        {
            if (QThread::currentThread() != nullptr)
            {
                moveToThread(QThread::currentThread());
                m_children->moveToThread(QThread::currentThread());
            }
            else
            {
                if (QCoreApplication::instance() != nullptr && QCoreApplication::instance()->thread() != nullptr)
                {
                    moveToThread(QCoreApplication::instance()->thread());
                    m_children->moveToThread(QCoreApplication::instance()->thread());
                }
            }
        }
    }
}

VG::QResource::QResource(QObject* parent) :
    QResource(QResourceIdentifier(), parent)
{
}

VG::QResource::~QResource()
{
    disconnect(this, SIGNAL(asyncSetParent(QObject*)), this, SLOT(onAsyncSetParent(QObject*)));
}

void VG::QResource::onAsyncSetParent(QObject* parent)
{
    setParent(parent);
    if (parent != nullptr && this->parent() == nullptr)
    {
        //throw std::runtime_error("Failed to set parent for QResource!");
        printf("FAILED TO SET A PARENT FOR QRESOURCE!\n");
    }

    if (m_children->parent() == nullptr)
    {
        m_children->setParent(this);
        if (m_children->parent() == nullptr)
        {
            //throw std::runtime_error("Failed to set parent for QResource's children list!");
            printf("FAILED TO SET A PARENT FOR QRESOURCE'S CHILDREN!\n");
        }
    }
}

void VG::QResourceList::onChildItemsChanged()
{
    emit itemsChanged();
}

VG::QResourceList::QResourceList(bool cppOwned, QObject* parent) :
    QResourceListBase(parent)
{
    if (cppOwned)
    {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    }
}

VG::QResourceList::QResourceList(QObject* parent) :
    VG::QResourceList(true, parent)
{
}

void VG::QResourceList::appendItem(VG::QResource *child)
{
    QResourceListBase::appendItem(child);
    connect(child->get_children(), SIGNAL(itemsChanged()), this, SLOT(onChildItemsChanged()));
}

int VG::QResourceList::removeItem(VG::QResource *child)
{
    int retval = QResourceListBase::removeItem(child);
    if (retval > 0)
        disconnect(child->get_children(), SIGNAL(itemsChanged()), this, SLOT(onChildItemsChanged()));

    return retval;
}

VG::QResource *VG::QInventory::internalQueryResourceByID(VG::QResource *root, const QNgvaID &id)
{
    if (root == nullptr || id == QNgvaID().UNDEFINED())
        return nullptr;

    if (root->get_identifier().id() == id)
        return root;

    VG::QResource* child = nullptr;
    for (int i=0; (child = root->get_children()->at(i)) != nullptr; i++)
    {
        QResource* retval = internalQueryResourceByID(child, id);
        if (retval != nullptr)
            return retval;
    }

    return nullptr;
}

VG::QResource *VG::QInventory::internalQueryResourceByName(VG::QResource *root, const QString &name)
{
    if (root == nullptr)
        return nullptr;

    if (root->get_identifier().type() == name)
        return root;

    VG::QResource* child = nullptr;
    for (int i=0; (child = root->get_children()->at(i)) != nullptr; i++)
    {
        QResource* retval = internalQueryResourceByName(child, name);
        if (retval != nullptr)
            return retval;
    }

    return nullptr;
}

void VG::QInventory::internalClassificationFilter(VG::QResource *root, const QString &classification, VG::QResourceList *list)
{
    if (root == nullptr)
        return;

    if (root->get_identifier().description() == classification)
        list->appendItem(root);

    VG::QResource* child = nullptr;
    for (int i=0; (child = root->get_children()->at(i)) != nullptr; i++)
        internalClassificationFilter(child, classification, list);
}

VG::QInventory::QInventory(
        const Database::DataSet& dataset,
        const char* stationName,
        int vehicleID,
        const char* identifiresPath,
        QObject *parent) :
    QResource(parent),
    m_dataset(dataset),
    m_vehicleID(vehicleID),
    m_equipment(vehicleID, identifiresPath)
{
    if (std::strlen(stationName) == 0)
        throw std::invalid_argument("systemType");

    if (dataset.Empty() == true)
        throw std::invalid_argument("dataset");

    m_systemIdentifier = m_equipment.QueryResourceIdentifier(stationName);
    if (m_systemIdentifier == nullptr)
        throw std::invalid_argument("systemType");
}

VG::QInventory::QInventory(QObject *parent) :
    QResource(parent),
    m_dataset(nullptr),
    m_vehicleID(-1)
{
}

VG::QResource* VG::QInventory::queryResourceByID(const QNgvaID &id)
{
    std::lock_guard<std::mutex> locker(m_mutex);
    return internalQueryResourceByID(this, id);
}

VG::QResource* VG::QInventory::queryResourceByName(const QString &name)
{
    std::lock_guard<std::mutex> locker(m_mutex);
    return internalQueryResourceByName(this, name);
}

VG::QResourceList* VG::QInventory::classificationFilter(const QString &classification)
{
    std::lock_guard<std::mutex> locker(m_mutex);

    // Return value should not be owned by CPP as it might cause memory leak
    // since the QML won't colllect it
    QResourceList* retval = new QResourceList(false, nullptr);

    internalClassificationFilter(this, classification, retval);
    return retval;
}
