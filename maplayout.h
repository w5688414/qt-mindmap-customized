#ifndef MAPLAYOUT_H
#define MAPLAYOUT_H


#include <QObject>
#include <QMap>
#include <QVector>
#include <QPointF>
#include <QRectF>

class MapLayout : public QObject
{
    Q_OBJECT
public:

    explicit MapLayout(QObject *parent = 0);

    void FixAllPositions();

    void Add(QWidget* node, QWidget* parent);
    void Remove(QWidget* node);

    QRectF GetPos(QWidget* node);

signals:
    void OnElementPosition(QWidget* node, QPointF newPos);

private:
    int UpdateAndGetNodeSize(QWidget* node, int topPos, int parentLeftPos, QMap<QWidget*, QPointF>& res);
    void EmitPositionCallbacks(QWidget* node, const QMap<QWidget*, QPointF> &res);

    struct Block {
        QWidget* parent;
        QVector<QWidget*> nodes;
        QVector<int> nodeVertPos;
        QPointF pos;
    };

    QMap<QWidget*, Block> m_blocks;
    QMap<QWidget*, QRectF> m_posCache;
};

#endif // MAPLAYOUT_H
