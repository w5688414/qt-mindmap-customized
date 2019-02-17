#ifndef MAPCONTEXTWIDGET_H
#define MAPCONTEXTWIDGET_H


#include <QObject>
#include <QSet>
#include <QFrame>
#include <QMap>

QT_BEGIN_NAMESPACE
class QGraphicsScene;
class QGraphicsView;
class QGraphicsPathItem;
QT_END_NAMESPACE

class MapNodeWidget;
class MapLayout;

#include "maplayout.h"

namespace MindMap {
class MapContext;
class MapCursor;
class MapNode;
}

class MapContextWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MapContextWidget(QWidget* parent = NULL, MindMap::MapContext* model = NULL);

    void keyPressEvent(QKeyEvent* ev) Q_DECL_OVERRIDE;
    bool focusNextPrevChild(bool) Q_DECL_OVERRIDE;

    MindMap::MapContext*  GetMapContext();

public slots:
    void OnNodePosition(QWidget* node, QPointF pos);
    void OnNodeAdded(MindMap::MapNode* node, MindMap::MapContext* caller);
    void OnNodeDeleted(MindMap::MapNode* node, MindMap::MapContext* caller);
    void OnChangeFocusUserRequest(MapNodeWidget* widget);
    void OnNodeKeypress(QKeyEvent* ev);
    void OnNodeResize(QWidget* node, QSize newSize);

private:
    QPainterPath GenPath(MindMap::MapNode* parent, MindMap::MapNode* child);
    void CreateNodeAtCursor();
    void DeleteNodeAtCursor();
    void MoveCursor(bool isUp, bool isDown, bool isLeft, bool isRight);
    void FoldNode(MindMap::MapNode* root, bool isFold);
    void UnfoldNode(MindMap::MapNode* root);
    void FocusOnNode(MindMap::MapNode *node);

    QGraphicsScene* m_nodeScene;
    MindMap::MapContext* m_context;
    MindMap::MapCursor* m_cursor;
    MapLayout* m_layout;

    QGraphicsView* m_nodeView;

    QMap<MindMap::MapNode*, MapNodeWidget*> m_nodeWidgets;
    QMap<MapNodeWidget*, QGraphicsPathItem*> m_pathWidgets;
    QSet<MindMap::MapNode*> m_nodesFolded;
};

#endif // MAPCONTEXTWIDGET_H
