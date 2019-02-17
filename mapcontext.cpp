#include "mapcontext.h"
#include "mapnode.h"

#include <QJsonObject>
#include <QJsonObject>
#include <QJsonArray>

namespace MindMap {

MapContext::MapContext()
{
    AddNode(NULL);

}

MapContext::MapContext(const QJsonObject &json)
{
    MapNode* root = AddNode(NULL);
    Deserialize(json, root);
}

MapNode *MapContext::AddNode(MapNode *parent)
{
    const bool isRoot = (parent == NULL && m_nodes.empty());
    if (!isRoot)
        Q_ASSERT(m_nodes.contains(parent));

    MapNode* newNode = new MapNode("");
    m_nodes.push_back(newNode);
    m_parents[newNode] = parent;
    m_childs[parent].push_back(newNode);
    emit OnNodeAdded(newNode, this);
    return newNode;
    // TODO: make undo
}

void MapContext::RemoveNode(MapNode *node)
{
    emit OnNodeDeleted(node, this);

    m_nodes.removeAll(node);
    MapNode* parent = m_parents[node];
    m_childs[parent].removeAll(node);
    m_parents.remove(node);
    if (m_childs.contains(node))
    {
        Q_FOREACH(MapNode* child, m_childs[node])
        {
            RemoveNode(child);
        }
        m_childs.remove(node);
    }
    // TODO: make undo
}

MapNode *MapContext::GetRootNode()
{
    if (m_nodes.empty())
        return NULL;

    return m_nodes.first();
}

MapNode *MapContext::GetNodeParent(MapNode *node)
{
    if (node == NULL)
        return NULL;

    if (!m_parents.contains(node))
        return NULL;

    return m_parents[node];
}

MapNode *MapContext::GetNodeFirstChild(MapNode *parent)
{
    if (parent == NULL)
        return NULL;

    if (!m_childs.contains(parent))
        return NULL;

    if (m_childs[parent].empty())
        return NULL;

    return m_childs[parent].first();
}

MapNode *MapContext::GetNodeLastChild(MapNode *parent)
{
    if (parent == NULL)
        return NULL;

    if (!m_childs.contains(parent))
        return NULL;

    if (m_childs[parent].empty())
        return NULL;

    return m_childs[parent].last();
}

MapNode *MapContext::GetNextSibling(MapNode *node)
{
    if (node == NULL)
        return NULL;

    if (!m_parents.contains(node))
        return NULL;

    MapNode* parent = m_parents[node];

    if (!m_childs.contains(parent))
        return NULL;

    const QVector<MapNode*>& siblings = m_childs[parent];
    int index = siblings.indexOf(node);

    if (index == -1 || index == siblings.size() - 1)
        return NULL;

    return siblings[index + 1];
}

MapNode *MapContext::GetPrevSibling(MapNode *node)
{
    if (node == NULL)
        return NULL;

    if (!m_parents.contains(node))
        return NULL;

    MapNode* parent = m_parents[node];

    if (!m_childs.contains(parent))
        return NULL;

    const QVector<MapNode*>& siblings = m_childs[parent];
    int index = siblings.indexOf(node);

    if (index == -1 || index == 0)
        return NULL;

    return siblings[index - 1];
}

const QVector<MapNode *>& MapContext::GetNodes()
{
    return m_nodes;
}

QJsonObject MapContext::GetJson(MapNode* node)
{
    if (node == NULL)
        node = m_nodes.front();

    QJsonObject res;
    res["title"] = QJsonValue(node->GetText());
    res["id"] = m_nodes.indexOf(node) + 1;
    QJsonObject ideas;
    if (m_childs.contains(node)) {
        int idx = 1;
        Q_FOREACH(MapNode* child, m_childs[node]) {
            ideas[ QString("%1").arg(idx++) ] = GetJson(child);
        }
        res["ideas"] = ideas;
    }

    return res;
}

void MapContext::Deserialize(const QJsonObject &json, MapNode* node)
{
    const QString title = json["title"].toString("");
    node->SetText(title);

    QJsonObject childs = json["ideas"].toObject();
    Q_FOREACH(QString key, childs.keys()) {
        MapNode* newNode = AddNode(node);
        Deserialize(childs[key].toObject(), newNode);
    }
}


}


