#ifndef MAPCURSOR_H
#define MAPCURSOR_H


#include <QObject>
#include <QMap>

namespace MindMap {

class MapContext;
class MapNode;

class MapCursor : public QObject
{
    Q_OBJECT

public:
    MapCursor(MapContext* context);

    enum CursorDirection {
        CD_Up,
        CD_Down,
        CD_Left,
        CD_Right,

        CD_Count
    };

    void SetNode(MapNode* node);
    MapNode* GetNode();
    void MoveCursor(CursorDirection dir);
    MapNode* CreateChildNode();
    void DeleteCurrentNode();

signals:
    void OnNodeFocusChanged(MapNode* node);

public slots:

private:
    MapNode* MoveVertical(MapNode* node, bool isUp);
    MapNode* GetCached(MapNode* node, CursorDirection cd);
    void AddCached(MapNode* nodeFrom, CursorDirection cd, MapNode* cachedTo);
    void ClearCached(MapNode* node);
    void UpdateCurNodeCache(MapNode** cache, MapNode* curNode);

    MapContext* m_context;
    MapNode* m_node;

    QMap< QPair<MapNode*, CursorDirection>, MapNode* > m_dirCache;
    MapNode* m_curNodeCache[CD_Count];
};

}

#endif // MAPCURSOR_H
