#ifndef MAPNODE_H
#define MAPNODE_H

#include <QString>
namespace MindMap {
   class MapNode
   {

   public:
       MapNode(const QString& text)
               : m_text(text)
           {}
       QString GetText() const { return m_text; }
       void SetText(const QString& text) { m_text = text; }
   private:
       QString m_text;

   };

}

#endif // MAPNODE_H
