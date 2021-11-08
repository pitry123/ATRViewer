#pragma once
#include <QObject>
#include <QMetaType>

struct Guid
{
    Q_GADGET
    Q_PROPERTY(long long MSB MEMBER MSB)
    Q_PROPERTY(long long LSB MEMBER LSB)

public:
    long long MSB;
    long long LSB;

    explicit Guid() :
        MSB(0.0), LSB(0.0)
    {
    }

    explicit Guid(long long _MSB, long long _LSB) :
        MSB(_MSB), LSB(_LSB)
    {
    }

    Guid(const Guid& other) :
        MSB(other.MSB), LSB(other.LSB)
    {
    }

    virtual ~Guid()
    {
    }

    Guid& operator=(const Guid& other)
    {
        MSB = other.MSB;
        LSB = other.LSB;

        return *this;
    }

    bool operator!=(const Guid& other) const
    {
        return (MSB != other.MSB ||
                LSB != other.LSB);
    }

    bool operator==(const Guid& other) const
    {
        return !(*this != other);
    }
};
Q_DECLARE_METATYPE(Guid)

struct BBoxParams {
        Q_GADGET
        Q_PROPERTY(double x  MEMBER x)
        Q_PROPERTY(double y  MEMBER y)
        Q_PROPERTY(double width  MEMBER width)
        Q_PROPERTY(double height  MEMBER height)
        Q_PROPERTY(int classification MEMBER classification)
        Q_PROPERTY(double range MEMBER range)
        Q_PROPERTY(double idTrack MEMBER idTrack)
        Q_PROPERTY(QString color MEMBER color)

    public:
        double x;
        double y;
        double width;
        double height;
        int classification;
        double range;
        double idTrack;
        QString color;

        explicit BBoxParams() :
            x(0.0), y(0.0), width(0.0), height(0.0), classification(0), range(0.0), idTrack(0.0), color("white") {
        }

        explicit BBoxParams(double _x, double _y, double _height, double _width, int _classification, double _range, double _idTrack, QString _color) :
            x(_x), y(_y), width(_width), height(_height), classification(_classification), range(_range), idTrack(_idTrack), color(_color) {
        }

        BBoxParams(const BBoxParams &other) :
            x(other.x), y(other.y), width(other.width), height(other.height),
            classification(other.classification), range(other.range), idTrack(other.idTrack), color(other.color) {
        }

        virtual ~BBoxParams() {
        }

        BBoxParams &operator=(const BBoxParams &other) {
            x = other.x;
            y = other.y;
            width = other.width;
            height = other.height;
            classification = other.classification;
            range = other.range;
            idTrack = other.idTrack;
            color = other.color;
            return *this;
        }

        bool operator!=(const BBoxParams &other) const {
            return (x != other.x ||
                    y != other.y ||
                    width != other.width ||
                    height != other.height ||
                    classification != other.classification);
        }

        bool operator==(const BBoxParams &other) const {
            return !(*this != other);
        }
};
Q_DECLARE_METATYPE(BBoxParams)


namespace AtrViewer
{
    class QGuiTypes : public QObject
    {
        Q_OBJECT

    public:
        explicit QGuiTypes(QObject* parent = nullptr);
    };
}
