#include "maplayout.h"

#include "maplayout.h"
#include "qwidget.h"

MapLayout::MapLayout(QObject *parent) : QObject(parent)
{
    m_blocks[NULL] = Block();
}

void MapLayout::Add(QWidget* node, QWidget* parent)
{
    Block& blockFrom = m_blocks[parent];
    blockFrom.nodes.append(node);
}

void MapLayout::Remove(QWidget* node)
{
    QMapIterator<QWidget*, Block> it(m_blocks);
    while (it.hasNext()) {
        it.next();
        if (it.value().nodes.contains(node))
        {
            m_blocks[it.key()].nodes.removeAll(node);
        }
    }
}

QRectF MapLayout::GetPos(QWidget *node)
{
    if (m_posCache.contains(node))
        return m_posCache[node];
    return QRectF();
}

void MapLayout::FixAllPositions()
{
    Q_ASSERT(m_blocks[NULL].nodes.empty() == false);
    QWidget*& rootNode = m_blocks[NULL].nodes.first();
    QMap<QWidget*, QPointF> res;
    UpdateAndGetNodeSize(rootNode, 0, 0, res);
    EmitPositionCallbacks(rootNode, res);
}

void MapLayout::EmitPositionCallbacks(QWidget* node, const QMap<QWidget*, QPointF>& res)
{
    if (!node->isVisible())
        return;

    QMap<QWidget*, Block>::iterator blockIt = m_blocks.find(node);

    if (res.contains(node))
        emit OnElementPosition(node, res[node]);

    if (blockIt != m_blocks.end()) {
        Block& block = blockIt.value();
        for(int childIdx = 0; childIdx < block.nodes.size(); ++childIdx) {
            EmitPositionCallbacks(block.nodes[childIdx], res);
        }
    }
}

int MapLayout::UpdateAndGetNodeSize(QWidget* node, int topPos, int parentLeftPos, QMap<QWidget*, QPointF>& res)
{
    QMap<QWidget*, Block>::iterator blockIt = m_blocks.find(node);
    int size = 0;
    QPointF newPos(parentLeftPos, topPos);

    if (blockIt != m_blocks.end()) {

        Block& block = blockIt.value();
        const int childLeftPos = parentLeftPos + node->geometry().width() + 50;
        int childrenSize = 0;

        for(int childIdx = 0; childIdx < block.nodes.size(); ++childIdx) {
            if (!block.nodes[childIdx]->isVisible())
                continue;

            const int childTop = topPos + childrenSize;
            const int childSize = UpdateAndGetNodeSize(block.nodes[childIdx], childTop, childLeftPos, res);
            childrenSize += childSize;
        }

        if (childrenSize < node->geometry().height())
            childrenSize = node->geometry().height();

        newPos = QPointF(parentLeftPos, topPos + childrenSize/2);
        block.pos = QPointF(childLeftPos, topPos);

        size = childrenSize;
    }
    else
    {
        size = node->geometry().height();
        newPos = QPointF(parentLeftPos, topPos + size/2);
    }

    const int minimumBlockSize = 3;
    // if it hasn't children
    if (size < minimumBlockSize)
        size = minimumBlockSize;

    m_posCache[node] = QRectF(newPos, node->geometry().size());
    res[node] = newPos;

    return size;
}

