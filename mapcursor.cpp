#include "mapcursor.h"
#include "mapcontext.h"
#include "mapnode.h"

namespace MindMap {

MapCursor::MapCursor(MapContext* context)
    : m_context(context)
    , m_node(m_context->GetRootNode())
{
    for(int cd = 0; cd < CD_Count; cd++)
        m_curNodeCache[cd] = NULL;
}


void MindMap::MapCursor::SetNode(MindMap::MapNode *node)
{
    m_node = node;

    UpdateCurNodeCache(m_curNodeCache, m_node);

    emit OnNodeFocusChanged(m_node);

}

MapNode *MapCursor::GetNode()
{
    return m_node;
}

MapNode* MapCursor::MoveVertical(MapNode* node, bool isUp)
{
    if (node == NULL)
        return NULL;

    MapNode* sibling = (isUp ? m_context->GetPrevSibling(node) : m_context->GetNextSibling(m_node));
    if (!sibling)
    {
        MapNode* parent = m_context->GetNodeParent(node);
        MapNode* parentSibl = (isUp ? m_context->GetPrevSibling(parent) : m_context->GetNextSibling(parent));
        if (parentSibl) {
            MapNode* parentSiblChild = (isUp ? m_context->GetNodeLastChild(parentSibl) : m_context->GetNodeFirstChild(parentSibl));
            if (parentSiblChild)
                return parentSiblChild;
            else
                return parentSibl;

        } else {
            return NULL;
        }
    }
    else {
        return sibling;
    }

    return NULL;
}

MapNode *MapCursor::GetCached(MapNode *node, MapCursor::CursorDirection cd)
{
    QPair<MapNode*, CursorDirection> key = QPair<MapNode*, CursorDirection>(node, cd);
    QMap< QPair<MapNode*, CursorDirection>, MapNode* >::iterator it = m_dirCache.find(key);
    if (it != m_dirCache.end())
        return it.value();
    else
        return NULL;
}

void MapCursor::AddCached(MapNode *nodeFrom, MapCursor::CursorDirection cd, MapNode *cachedTo)
{
    if (nodeFrom == NULL || cd == CD_Count || cachedTo == NULL)
        return;

    m_dirCache[ QPair<MapNode*, CursorDirection>(nodeFrom, cd) ] = cachedTo;
}

void MapCursor::ClearCached(MapNode *node)
{
    QMutableMapIterator< QPair<MapNode*, CursorDirection>, MapNode* > it(m_dirCache);
    while(it.hasNext()) {
        it.next();
        if (it.key().first == node || it.value() == node) {
            it.remove();
        }
    }

    for(int cd = 0; cd < CD_Count; cd++)
        if (m_curNodeCache[cd] == node)
            m_curNodeCache[cd] = NULL;
}

void MapCursor::UpdateCurNodeCache(MapNode** cache, MapNode* curNode)
{
    if (curNode == NULL)
        return;

    for(int cd = 0; cd < CD_Count; cd++)
        cache[cd] = GetCached(curNode, static_cast<CursorDirection>(cd));

    if (cache[CD_Up] == NULL) {
        cache[CD_Up] = m_context->GetPrevSibling(curNode);
    }

    if (cache[CD_Down] == NULL) {
        cache[CD_Down] = m_context->GetNextSibling(curNode);
    }

    if (cache[CD_Left] == NULL) {
        cache[CD_Left] = m_context->GetNodeParent(curNode);
    }

    if (cache[CD_Right] == NULL) {
        cache[CD_Right] = m_context->GetNodeFirstChild(curNode);
    }
}

void MindMap::MapCursor::MoveCursor(MindMap::MapCursor::CursorDirection dir)
{
    MapNode* forcedMove[CD_Count]; // non-cached, but handy movements
    for(int cd = 0; cd < CD_Count; cd++)
        forcedMove[cd] = NULL;

    UpdateCurNodeCache(m_curNodeCache, m_node);

    if (m_curNodeCache[CD_Up] == NULL)
        forcedMove[CD_Up] = MoveVertical(m_node, true);
    if (m_curNodeCache[CD_Down] == NULL)
        forcedMove[CD_Down] = MoveVertical(m_node, false);

    const bool isForced = (m_curNodeCache[dir] == NULL) && (forcedMove[dir] != NULL);
    MapNode* newNode = (isForced ? forcedMove[dir] : m_curNodeCache[dir]);
    if (newNode)
    {
        MapNode* oldNode = m_node;

        CursorDirection inverseDir =
                (dir == CD_Up ? CD_Down :
                    (dir == CD_Down ? CD_Up :
                        (dir == CD_Left ? CD_Right :
                            (dir == CD_Right ? CD_Left :
                                CD_Count))));

        if (!isForced)
            AddCached(newNode, inverseDir, oldNode);

        UpdateCurNodeCache(m_curNodeCache, newNode);

        m_node = newNode;
        emit OnNodeFocusChanged(m_node);
    }
}

MindMap::MapNode *MindMap::MapCursor::CreateChildNode()
{
    if (!m_node)
        return NULL;

    MapNode* newNode = m_context->AddNode(m_node);
    return newNode;
}

void MindMap::MapCursor::DeleteCurrentNode()
{
    MapNode* parentNode = m_context->GetNodeParent(m_node);
    if (parentNode == NULL)
        return;

    ClearCached(m_node);
    ClearCached(m_curNodeCache[CD_Right]); // TODO: clear for all children recoursively

    m_context->RemoveNode(m_node);

    m_node = NULL;
}

}

