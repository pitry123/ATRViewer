#pragma once
#include "ProjectCommon.h"
#include "Helpers/QQmlAutoPropertyHelpers.h"
#include "Helpers/Constants.h"

#include <Database.hpp>

#include <QObject>
#include <QThread>
#include <QVector>
#include <QVariant>

#include <limits>
#include <mutex>

class QNgvaID
{
    Q_GADGET
    Q_PROPERTY(int resourceID READ resourceID)
    Q_PROPERTY(int instanceID READ instanceID)

private:
    NGVA::P_LDM_Common::T_IdentifierType m_id;

public:
    static const QNgvaID UNDEFINED()
    {
        return QNgvaID();
    }

    QNgvaID(const NGVA::P_LDM_Common::T_IdentifierType& id) :
        m_id(id)
    {
    }

    QNgvaID(int32_t _resource_id, int32_t _instance_id) :
        QNgvaID(NGVA::P_LDM_Common::T_IdentifierType{ _resource_id, _instance_id })
    {
    }

    QNgvaID() :
        QNgvaID(-1, -1)
    {
    }

    QNgvaID(const QNgvaID& other) :
        m_id(other.m_id)
    {
    }

    QNgvaID& operator=(const QNgvaID& other)
    {
        m_id = other.m_id;
        return *this;
    }

    bool operator==(const QNgvaID& other) const
    {
        return (std::memcmp(&m_id, &(other.m_id), sizeof(NGVA::P_LDM_Common::T_IdentifierType)) == 0);
    }

    bool operator!=(const QNgvaID& other) const
    {
        return !(*this == other);
    }

    operator NGVA::P_LDM_Common::T_IdentifierType() const
    {
        return m_id;
    }

    int resourceID() const
    {
        return static_cast<int>(m_id.A_resourceId);
    }

    int instanceID() const
    {
        return static_cast<int>(m_id.A_instanceId);
    }
};
Q_DECLARE_METATYPE(QNgvaID)

class QResourceIdentifier
{
    Q_GADGET
    Q_PROPERTY(QNgvaID id READ id)
    Q_PROPERTY(int vehicleID READ vehicleID)
    Q_PROPERTY(int componentID READ componentID)
    Q_PROPERTY(int equipmentID READ equipmentID)
    Q_PROPERTY(int moduleID READ moduleID)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(QString description READ description)
    Q_PROPERTY(int displayOrder READ displayOrder)

    Q_PROPERTY(QResourceIdentifier parent READ parent)

private:
    static constexpr int32_t UNDEFINED_VALUE = -1;
    NGVA::VG::ResourceIdentifier m_identifier;

public:
    QResourceIdentifier() // Empty
    {
    }

    QResourceIdentifier(const NGVA::VG::ResourceIdentifier& identifier) :
        m_identifier(identifier)
    {
    }

    QResourceIdentifier(const NGVA::P_Vehicle_Configuration_PSM::C_Resource_Specification& specification) :
        QResourceIdentifier(NGVA::VG::ResourceIdentifier(specification))
    {
    }

    QResourceIdentifier(const QResourceIdentifier& other) :
        m_identifier(other.m_identifier)
    {
    }

    QResourceIdentifier operator=(const QResourceIdentifier& other)
    {
        m_identifier = other.m_identifier;
        return *this;
    }

    bool operator==(std::nullptr_t) const
    {
        return (m_identifier.Empty() == true);
    }

    bool operator!=(std::nullptr_t) const
    {
        return !(*this == nullptr);
    }

    bool operator==(const QResourceIdentifier& other) const
    {
        if (m_identifier.Empty() == true && other.m_identifier.Empty() == true)
            return true;

        if (m_identifier.Empty() == true || other.m_identifier.Empty() == true)
            return false;

        return (m_identifier.Specification().A_sourceID.A_resourceId == other.m_identifier.Specification().A_sourceID.A_resourceId);
    }

    bool operator!=(const QResourceIdentifier& other) const
    {
        return !(*this == other);
    }

    QNgvaID id() const
    {
        if (m_identifier.Empty() == true)
            return QNgvaID::UNDEFINED();

        return m_identifier.Specification().A_sourceID;
    }

    int vehicleID() const
    {
        if (m_identifier.Empty() == true)
            return static_cast<int>(UNDEFINED_VALUE);

        return static_cast<int>(m_identifier.VehicleID());
    }

    int componentID() const
    {
        if (m_identifier.Empty() == true)
            return static_cast<int>(UNDEFINED_VALUE);

        return static_cast<int>(m_identifier.ComponentID());
    }

    int equipmentID() const
    {
        if (m_identifier.Empty() == true)
            return static_cast<int>(UNDEFINED_VALUE);

        return static_cast<int>(m_identifier.EquipmentID());
    }

    int moduleID() const
    {
        if (m_identifier.Empty() == true)
            return static_cast<int>(UNDEFINED_VALUE);

        return static_cast<int>(m_identifier.ModuleID());
    }

    QString type() const
    {
        if (m_identifier.Empty() == true)
            return "Undefined";

        return QString(m_identifier.Specification().A_resourceTypeName.str);
    }

    QString description() const
    {
        if (m_identifier.Empty() == true)
            return "Undefined";

        return QString(m_identifier.Specification().A_description.str);
    }

    int displayOrder() const
    {
        if (m_identifier.Empty() == true)
            return (std::numeric_limits<int>::max)();

        return static_cast<int>(m_identifier.DisplayOrder());
    }

    QResourceIdentifier parent() const
    {
        if (m_identifier.Empty() ==true)
            return QResourceIdentifier();

        NGVA::VG::ResourceIdentifier parent = m_identifier.Parent();
        if (parent.Empty() == true)
            return QResourceIdentifier();

        return QResourceIdentifier(parent);
    }

    const NGVA::P_Vehicle_Configuration_PSM::C_Resource_Specification& get_specifications() const
    {
        return m_identifier.Specification();
    }
};
Q_DECLARE_METATYPE(QResourceIdentifier)

namespace VG
{
class QResourceList;
class QResource : public QObject
{
    Q_OBJECT

private:
    QML_CONSTANT_AUTO_PROPERTY(QResourceIdentifier, identifier)
    QML_READONLY_AUTO_PROPERTY(VG::QResourceList*, children)
    QML_WRITABLE_AUTO_PROPERTY(QResource*, owner)

public:
    QResource(const QResourceIdentifier& identifier,QObject* parent = nullptr);
    explicit QResource(QObject* parent = nullptr);
    virtual ~QResource() override;

signals:
    void asyncSetParent(QObject* parent);

private slots:
    void onAsyncSetParent(QObject* parent);
};

QML_LIST_CLASS(QResourceListBase, VG::QResource)
class QResourceList : public QResourceListBase
{
    Q_OBJECT

private slots:
    void onChildItemsChanged();

public:
    QResourceList(bool cppOwned, QObject* parent);
    QResourceList(QObject* parent = nullptr);
    virtual void appendItem(QResource* child) override;
    virtual int removeItem(QResource* child) override;
};

class QInventory : public QResource
{
    Q_OBJECT

    QML_CONSTANT_AUTO_PROPERTY(QResourceIdentifier, systemIdentifier)

private:
    std::mutex m_mutex;
    Database::DataSet m_dataset;
    int32_t m_vehicleID;
    NGVA::VG::EquipmentModel m_equipment;

    QResource* internalQueryResourceByID(VG::QResource* root, const QNgvaID& id);
    VG::QResource* internalQueryResourceByName(QResource* root, const QString& name);
    void internalClassificationFilter(VG::QResource* root, const QString& classification, VG::QResourceList* list);

    template<typename T, typename... Args>
    VG::QResource* internalQueryOrCreateResource(
            const NGVA::P_Vehicle_Configuration_PSM::C_Resource_Specification& specifications,
            Args&&... args)
    {
        QResource* retval = internalQueryResourceByID(this, specifications.A_sourceID);
        if (retval != nullptr)
            return retval;

        NGVA::VG::ResourceIdentifier identifier = m_equipment.AddResourceIdentifier(specifications);
        if (identifier.Empty() == true)
            throw std::runtime_error("Failed to add resource");

        QResource* parent = nullptr;
        NGVA::VG::ResourceIdentifier parentIdentifier = identifier.Parent();
        if (parentIdentifier.Empty() == false)
            parent = internalQueryOrCreateResource<T>(parentIdentifier.Specification(), std::forward<Args>(args)...);
        else
            parent = this;

        if (identifier.ModuleID() == 0)
            retval = new QResource(QResourceIdentifier(identifier), parent);
        else
            retval = new T(QResourceIdentifier(identifier), std::forward<Args>(args)...);

        parent->get_children()->appendItem(retval);
        return retval;
    }

public:
    QInventory(
                const Database::DataSet& dataset,
                const char* stationName,
                int vehicleID,
                const char* identifiresPath,
                QObject* parent = nullptr);
    explicit QInventory(QObject* parent = nullptr);

    // Single item queries
    Q_INVOKABLE VG::QResource* queryResourceByID(const QNgvaID& id);
    Q_INVOKABLE VG::QResource* queryResourceByName(const QString& name);

    // Multi Items queries
    Q_INVOKABLE VG::QResourceList* classificationFilter(const QString& classification);

    // Creation
    template<typename T, typename... Args>
    VG::QResource* queryOrCreateResource(
            const NGVA::P_Vehicle_Configuration_PSM::C_Resource_Specification& specifications,
            Args&&... args)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        return internalQueryOrCreateResource<T>(specifications, std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    VG::QResource* queryOrCreateResource(
            const QNgvaID& id,
            Args&&... args)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        NGVA::VG::ResourceIdentifier identifier = m_equipment.QueryResourceIdentifier(id);
        if (identifier.Empty() == true)
            return nullptr; //throw std::runtime_error("Failed to add resource");

        return internalQueryOrCreateResource<T>(identifier.Specification(), std::forward<Args>(args)...);
    }

    template<typename T, typename... Args>
    VG::QResource* queryOrCreateResource(
            const char* name,
            Args&&... args)
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        NGVA::VG::ResourceIdentifier identifier = m_equipment.QueryResourceIdentifier(name);
        if (identifier.Empty() == true)
            return nullptr; //throw std::runtime_error("Failed to add resource");

        return internalQueryOrCreateResource<T>(identifier.Specification(), std::forward<Args>(args)...);
    }
};
}
