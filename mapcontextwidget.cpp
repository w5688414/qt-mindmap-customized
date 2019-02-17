#include "mapcontextwidget.h"
#include "mapnodewidget.h"
#include "mapnode.h"
#include "mapcontext.h"
#include "mapcursor.h"
#include "maplayout.h"

#include <QLabel>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QKeyEvent>

MapContextWidget::MapContextWidget(QWidget *parent, MindMap::MapContext* model)
    : QFrame(parent)
    , m_nodeScene(new QGraphicsScene)
    , m_context(model)
    , m_layout(new MapLayout)
{
    m_nodeView = new QGraphicsView(m_nodeScene);
    m_nodeView->setDragMode(QGraphicsView::ScrollHandDrag);
    m_nodeView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    m_nodeView->setInteractive(true);
    // m_nodeView->setRenderHint(QPainter::Antialiasing, true);

    // Layout: maximize to parent window
    QGridLayout* layout = new QGridLayout(this);
    layout->setMargin(0);
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_nodeView);
    setLayout(layout);

    connect(m_layout, SIGNAL(OnElementPosition(QWidget*,QPointF)),
            this,     SLOT(OnNodePosition(QWidget*,QPointF)));

    // Context: mindmap node storage
    Q_ASSERT(m_context != NULL);
    if (m_context == NULL)
        m_context = new MindMap::MapContext;

    connect(m_context, SIGNAL(OnNodeAdded(MindMap::MapNode*,MindMap::MapContext*)),
            this,      SLOT(OnNodeAdded(MindMap::MapNode*,MindMap::MapContext*)));
    connect(m_context, SIGNAL(OnNodeDeleted(MindMap::MapNode*,MindMap::MapContext*)),
            this,      SLOT(OnNodeDeleted(MindMap::MapNode*,MindMap::MapContext*)));

    const QVector<MindMap::MapNode*> nodes = m_context->GetNodes();
    Q_FOREACH(MindMap::MapNode* node, nodes)
        OnNodeAdded(node, m_context);

    m_cursor = new MindMap::MapCursor(m_context);
    FocusOnNode(m_cursor->GetNode());
}

void MapContextWidget::keyPressEvent(QKeyEvent *ev)
{
    // Q_ASSERT(m_cursor->GetNode() == NULL);
    if(m_cursor->GetNode() != NULL)
        m_nodeWidgets[ m_cursor->GetNode() ]->OnKeypress(ev);
    else
        OnNodeKeypress(ev);
}

bool MapContextWidget::focusNextPrevChild(bool)
{
    // prevent internal qt logic with tab focus change
    return false;
}

MindMap::MapContext *MapContextWidget::GetMapContext()
{
    return m_context;
}

void MapContextWidget::OnNodeAdded(MindMap::MapNode* node, MindMap::MapContext* caller)
{
    // create node
    {
        MapNodeWidget* newWidget = new MapNodeWidget(node, NULL);

        connect(newWidget,  SIGNAL(OnChangeFocusUserRequest(MapNodeWidget*)),
                this,       SLOT(OnChangeFocusUserRequest(MapNodeWidget*)));
        connect(newWidget,  SIGNAL(OnKeypress(QKeyEvent*)),
                this,       SLOT(OnNodeKeypress(QKeyEvent*)));
        connect(newWidget,  SIGNAL(OnWidgetResize(QWidget*,QSize)),
                this,       SLOT(OnNodeResize(QWidget*,QSize)));

        m_nodeScene->addWidget(newWidget);
        m_nodeWidgets[node] = newWidget;
    }

    // create layout element
    {
        MindMap::MapNode* parent = caller->GetNodeParent(node);
        MapNodeWidget* parentWidget = (parent ? m_nodeWidgets[parent] : NULL);
        m_layout->Add(m_nodeWidgets[node], parentWidget);
        m_layout->FixAllPositions();
    }

}

void MapContextWidget::OnNodeDeleted(MindMap::MapNode* node, MindMap::MapContext*)
{
    if (m_nodeWidgets.contains(node))
    {
        MapNodeWidget* widget = m_nodeWidgets[node];

        m_layout->Remove( widget );

        if (m_pathWidgets.contains( widget ))
            delete m_pathWidgets[widget];
        m_pathWidgets.remove(widget);

        delete widget;
        m_nodeWidgets.remove(node);
    }

    m_layout->FixAllPositions();
}

void MapContextWidget::OnChangeFocusUserRequest(MapNodeWidget *widget)
{
    MindMap::MapNode* oldNode = m_cursor->GetNode();

    QMapIterator<MindMap::MapNode*, MapNodeWidget*> it(m_nodeWidgets);
    while(it.hasNext())
    {
        it.next();
        if (it.value() == widget)
        {
            MindMap::MapNode* newNode = it.key();
            if (newNode != oldNode)
                m_cursor->SetNode(newNode);
            break;
        }
    }
}

void MapContextWidget::OnNodeKeypress(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Tab)
    {
        CreateNodeAtCursor();
    }
    else if (ev->key() == Qt::Key_Delete)
    {
        DeleteNodeAtCursor();
    }
    else if (ev->key() == Qt::Key_Up ||
             ev->key() == Qt::Key_Down ||
             ev->key() == Qt::Key_Left ||
             ev->key() == Qt::Key_Right)
    {
        MoveCursor(
            ev->key() == Qt::Key_Up,
            ev->key() == Qt::Key_Down,
            ev->key() == Qt::Key_Left,
            ev->key() == Qt::Key_Right
        );
    }
    else if (ev->key() == Qt::Key_F)
    {
        MindMap::MapNode* curNode = m_cursor->GetNode();
        FoldNode( curNode, !m_nodesFolded.contains(curNode) );
    }
}

void MapContextWidget::OnNodeResize(QWidget *, QSize)
{
    m_layout->FixAllPositions();
}

void MapContextWidget::MoveCursor(bool isUp, bool isDown, bool isLeft, bool isRight)
{
    MindMap::MapNode* oldNode = m_cursor->GetNode();

    if (isUp)
        m_cursor->MoveCursor(MindMap::MapCursor::CD_Up);
    if (isDown)
        m_cursor->MoveCursor(MindMap::MapCursor::CD_Down);
    if (isLeft)
        m_cursor->MoveCursor(MindMap::MapCursor::CD_Left);
    if (isRight)
        m_cursor->MoveCursor(MindMap::MapCursor::CD_Right);

    MindMap::MapNode* newNode = m_cursor->GetNode();

    if (oldNode != newNode && m_nodeWidgets.contains(newNode))
    {
        if (!m_nodeWidgets[newNode]->isVisible())
            UnfoldNode(newNode);

        FocusOnNode(newNode);
    }
}

void MapContextWidget::UnfoldNode(MindMap::MapNode* root)
{
    do
    {
        if (root == m_context->GetRootNode())
            break;

        root = m_context->GetNodeParent(root);
    } while(!m_nodesFolded.contains(root));

    FoldNode(root, false);
}

void MapContextWidget::FocusOnNode(MindMap::MapNode *node)
{
    if (node == NULL) {
        setFocus();
    } else {
        m_nodeView->centerOn( m_nodeWidgets[node]->pos() );
        m_nodeWidgets[ node ]->setFocus();
    }
}

void MapContextWidget::FoldNode(MindMap::MapNode* root, bool isFold)
{
    if (root == NULL)
        return;

    MindMap::MapNode* itNode = root;

    bool foldedAtLeastOneNode = false;
    bool goBack = false;
    do
    {
        const bool isFoldedChild = m_nodesFolded.contains(itNode) && itNode != root;

        // Advance
        if (!goBack)
        {
            MindMap::MapNode* tryNode = NULL;

            if (!isFoldedChild)
                tryNode = m_context->GetNodeFirstChild(itNode);

            if (tryNode == NULL && itNode != root)
                tryNode = m_context->GetNextSibling(itNode);

            if (tryNode == NULL)
                goBack = true;
            else
                itNode = tryNode;
        }

        // Returned to beginning, quit
        if (goBack && itNode == root)
            break;

        // Return to parent
        if (goBack)
        {
            itNode = m_context->GetNodeParent(itNode);

            Q_ASSERT(itNode != NULL);

            if (itNode == root)
                break;

            MindMap::MapNode* parentSibling = m_context->GetNextSibling(itNode);
            if (parentSibling)
            {
                itNode = parentSibling;
                goBack = false;
            }
            else
                continue;
        }

        // Process: hide or show
        if (m_nodeWidgets.contains(itNode))
        {
            MapNodeWidget* w = m_nodeWidgets[itNode];
            w->setVisible( !isFold );

            if (m_pathWidgets.contains(w))
                m_pathWidgets[w]->setVisible( !isFold );

            foldedAtLeastOneNode = true;
        }
    } while(true);

    if (foldedAtLeastOneNode)
    {
        if (isFold)
            m_nodesFolded.insert(root);
        else
            m_nodesFolded.remove(root);

        m_nodeWidgets[root]->SetFolded(isFold);

        m_layout->FixAllPositions();
    }
}

void MapContextWidget::CreateNodeAtCursor()
{
    FoldNode(m_cursor->GetNode(), false);

    MindMap::MapNode* newNode = m_cursor->CreateChildNode();
    if (!newNode)
        return;
    m_cursor->SetNode(newNode);
    FocusOnNode(newNode);
}

void MapContextWidget::DeleteNodeAtCursor()
{
    m_cursor->DeleteCurrentNode();
    MindMap::MapNode* newNode = m_cursor->GetNode();
    if (m_nodeWidgets.contains(newNode))
    {
        FocusOnNode(newNode);
    }
    else
        FocusOnNode(NULL);
}

void MapContextWidget::OnNodePosition(QWidget *node, QPointF pos)
{
    MapNodeWidget* widget = dynamic_cast<MapNodeWidget*>(node);
    widget->move(pos.x(), pos.y());

    MindMap::MapNode* child = NULL;
    QMapIterator<MindMap::MapNode*, MapNodeWidget*> itChild(m_nodeWidgets);
    while (itChild.hasNext())
    {
        itChild.next();
        if (itChild.value() == widget)
            child = itChild.key();
    }

    MindMap::MapNode* parent = m_context->GetNodeParent(child);

    if (!parent)
        return;

    if (m_pathWidgets.contains(widget))
        m_pathWidgets[widget]->setPath(GenPath(parent, child));
    else
        m_pathWidgets.insert(m_nodeWidgets[child], m_nodeScene->addPath(GenPath(parent, child)));
}

QPainterPath MapContextWidget::GenPath(MindMap::MapNode* parent, MindMap::MapNode* child) //画连接线
{
    const float curvePointsOffset = 20.f;

    QRectF parentRect = m_layout->GetPos( m_nodeWidgets[parent] );
    QRectF childRect = m_layout->GetPos( m_nodeWidgets[child] );

    const QPointF from_1 = parentRect.topRight() + QPointF(0, parentRect.height() / 2);
    const QPointF from_2 = from_1 + QPointF(curvePointsOffset, 0.f);

    const QPointF to_1 = childRect.topLeft() + QPointF(0, childRect.height() / 2);
    const QPointF to_2 = to_1 - QPointF(curvePointsOffset, 0.f);

    QPainterPath path;
    path.moveTo(from_1);
    path.cubicTo(from_2, to_2, to_1);
    return path;
}

