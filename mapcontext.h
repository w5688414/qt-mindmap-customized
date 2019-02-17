#ifndef MAPCONTEXT_H
#define MAPCONTEXT_H

#include <QObject>
#include <QVector>
#include <QMap>

namespace MindMap {

class MapNode;
class MapContext;

class MapContext : public QObject
{
    Q_OBJECT

public:
    MapContext();
    MapContext(const QJsonObject& json);

    MapNode* AddNode(MapNode* parent);
    void RemoveNode(MapNode* node);

    MapNode* GetRootNode();
    MapNode* GetNodeParent(MapNode* node);
    MapNode* GetNodeFirstChild(MapNode* parent);
    MapNode* GetNodeLastChild(MapNode* parent);
    MapNode* GetNextSibling(MapNode* node);
    MapNode* GetPrevSibling(MapNode* node);

    const QVector<MapNode*>& GetNodes();

    QJsonObject GetJson(MapNode* node = NULL);

signals:
    void OnNodeAdded(MindMap::MapNode* node, MindMap::MapContext* caller);
    void OnNodeDeleted(MindMap::MapNode* node, MindMap::MapContext* caller);

private:
    void Deserialize(const QJsonObject &json, MapNode* node);

    QVector<MapNode*> m_nodes;
    QMap<MapNode*, MapNode*> m_parents;
    QMap<MapNode*, QVector<MapNode*> > m_childs;
};

}

#endif // MAPCONTEXT_H
