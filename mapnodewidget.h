#ifndef MAPNODEWIDGET_H
#define MAPNODEWIDGET_H


#include <QObject>
#include <QFrame>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QMouseEvent;
QT_END_NAMESPACE

namespace MindMap {
    class MapNode;
}

class MapNodeWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MapNodeWidget(MindMap::MapNode* node, QWidget* parent = NULL);
    ~MapNodeWidget();

    void SetText(const QString& text);
    void SetFolded(bool isFolded);
    void SetCustomStyle(const QString& text);

    void mousePressEvent(QMouseEvent* ev) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* ev) Q_DECL_OVERRIDE;
    void focusInEvent(QFocusEvent* ev) Q_DECL_OVERRIDE;
    bool focusNextPrevChild(bool) Q_DECL_OVERRIDE;

    void OnTextareaFocusChange(bool hasFocus);

signals:
    void OnChangeFocusUserRequest(MapNodeWidget*);
    void OnCursorMoveRequested(bool isUp, bool isDown, bool isLeft, bool isRight);
    void OnCursorCreateNodeRequested();
    void OnCursorRemoveNodeRequested();
    void OnKeypress(QKeyEvent* ev);
    void OnWidgetResize(QWidget* widget, QSize newSize);

private slots:
    void OnTextChanged();

private:
    void UpdateTextareaStyle();

    QPlainTextEdit* m_label;
    QSize m_size;
    bool m_isFolded;
    MindMap::MapNode* m_node;
    int row=0;
    int column=0;
};

#endif // MAPNODEWIDGET_H
