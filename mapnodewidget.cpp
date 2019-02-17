#include "mapnodewidget.h"
#include "mapnode.h"

#include <QTextEdit>
#include <QGridLayout>
#include <QPlainTextEdit>
#include<QVBoxLayout>
#include<QSpacerItem>

class NodeTextEdit : public QPlainTextEdit
{
    MapNodeWidget* m_parent;

public:
    explicit NodeTextEdit(MapNodeWidget *parent = 0)
        : QPlainTextEdit(parent)
        , m_parent(parent)
    {}

    void mousePressEvent(QMouseEvent* ev) Q_DECL_OVERRIDE
    {
        if (isReadOnly())
        {
            ev->ignore();
            return;
        }

        QPlainTextEdit::mousePressEvent(ev);
    }

    void keyPressEvent(QKeyEvent* ev) Q_DECL_OVERRIDE
    {
        update();

        if (isReadOnly() && ev->key() == Qt::Key_Space) {  //禁止按空格键后编辑功能
            setReadOnly(true);
        }
        else if (!isReadOnly() && ev->key() == Qt::Key_Escape) {
            setReadOnly(true);
        }
        else if (!isReadOnly() && ev->key() == Qt::Key_Tab) {
            setReadOnly(true);
            m_parent->keyPressEvent(ev);
        }
        else if (!isReadOnly() && ev->key() == Qt::Key_Return && (ev->modifiers() & Qt::ShiftModifier) == 0) {
            setReadOnly(true);
            m_parent->keyPressEvent(ev);
        }else if((ev->key() == Qt::Key_C || ev->key() == Qt::Key_X) &&
                 ev->modifiers() == Qt::ControlModifier){
            copy();

        }
        else if (!isReadOnly()) {
            QPlainTextEdit::keyPressEvent(ev);
        }
        else {
            m_parent->keyPressEvent(ev);
        }
    }

    bool focusNextPrevChild(bool) Q_DECL_OVERRIDE
    {
        // prevent internal qt logic with tab focus change
        return false;
    }

    void focusInEvent(QFocusEvent*) Q_DECL_OVERRIDE
    {
        setReadOnly(true);
        m_parent->OnTextareaFocusChange(true);
    }

    void focusOutEvent(QFocusEvent *) Q_DECL_OVERRIDE
    {
        setReadOnly(true);
        m_parent->OnTextareaFocusChange(false);
    }

};

MapNodeWidget::MapNodeWidget(MindMap::MapNode* node, QWidget* parent)
    : QFrame(parent)
    , m_isFolded(false)
    , m_node(node)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizeIncrement(10, 10);
    setFocusPolicy(Qt::ClickFocus);

    QGridLayout* layout = new QGridLayout(this);
    layout->setVerticalSpacing(22);

    QString addrStr = QString("0x%1").arg((size_t)this,sizeof(size_t)*2,16,QLatin1Char('0'));

    m_label = new NodeTextEdit(this);
    m_label->setPlaceholderText(addrStr);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_label->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_label->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_label->setReadOnly(true);
    m_label->setFocusPolicy(Qt::ClickFocus);
    m_label->document()->setPlainText(m_node->GetText());
    SetCustomStyle(m_node->GetText());

    layout->addWidget(m_label);
    layout->setMargin(0);
    layout->setContentsMargins(0, 6, 0, 3);  //依次是左 上 右 下的设置间距
//    layout->setSpacing(10);

//    connect(m_label, SIGNAL(textChanged()),
//            this, SLOT(OnTextChanged()));
//    this->setFrameShape(QFrame::Box);  //增加边框

    clearFocus();
    OnTextChanged();
}

void MapNodeWidget::SetCustomStyle(const QString& text)  //设置规则
{
    if(text.startsWith("解禁")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0 rgba(0, 196, 0, 255), stop:1 rgba(255, 255, 255, 255)); color : #1D1624;}"); //设置背景色 绿色
    }else if(text.startsWith("增减持")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : red; color : #1D1624; }"); //设置背景色 红色
    }else if(text.startsWith("大宗交易")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : green; color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("股东构成")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : rgba(255,0,0,50%); color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("业绩构成")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : #7CCD7C; color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("关注度")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : qlineargradient(spread:pad, x1:1, y1:1, x2:0, y2:0, stop:0 rgba(0, 196, 0, 255), stop:1 rgba(255, 255, 255, 255)); color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("年度封停次数")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : #7CCD7C; color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("前20日区间涨幅")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : #7CCD7C; color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("前20日日均振幅")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : #7CCD7C; color : #1D1624; }"); //设置背景色 绿色
    }else if(text.startsWith("基本面")){
        m_label->setStyleSheet("QPlainTextEdit { background-color : rgba(255,0,0,10%); color : #1D1624; }"); //设置背景色 绿色
    }


}

MapNodeWidget::~MapNodeWidget()
{
    delete m_label;
}

void MapNodeWidget::SetText(const QString &text)
{
    m_label->document()->setPlainText(text);
}

void MapNodeWidget::SetFolded(bool isFolded)
{
    m_isFolded = isFolded;
    UpdateTextareaStyle();
}

void MapNodeWidget::mousePressEvent(QMouseEvent *)
{
    emit OnChangeFocusUserRequest(this);
}

void MapNodeWidget::keyPressEvent(QKeyEvent *ev)
{
    // input keys are processed in label
    // control keys are processed in parent
    emit OnKeypress(ev);
}

void MapNodeWidget::focusInEvent(QFocusEvent*)
{
    // focus should be on label
    m_label->setFocus();
}

bool MapNodeWidget::focusNextPrevChild(bool)
{
    // prevent internal qt logic with tab focus change
    return false;
}

void MapNodeWidget::OnTextareaFocusChange(bool hasFocus)
{
    Q_UNUSED(hasFocus);
//    UpdateTextareaStyle();   //点击结点后，颜色变化功能
}

void MapNodeWidget::UpdateTextareaStyle()
{
    if (m_label->hasFocus())
    {
        if (!m_isFolded)  //鼠标焦点点击
            m_label->setStyleSheet("QPlainTextEdit { background-color : #D6B0FF; color : #1D1624; }");
        else
            m_label->setStyleSheet("QPlainTextEdit { background-color : #7c7385; color : #1D1624; }");
    }
    else
    {
        if (!m_isFolded)
            m_label->setStyleSheet("QPlainTextEdit { background-color : #FCFFFF; color : #1D1624; }");
        else
            m_label->setStyleSheet("QPlainTextEdit { background-color : #7ac483; color : #1f1f1b; }");
    }
}


void MapNodeWidget::OnTextChanged()
{
    const QString dataText = m_label->document()->toPlainText();

    m_node->SetText(dataText);


    const QFontMetrics fm(m_label->font());
    const QSize addSize(20, 20);  //设置每个结点的高
    const QSize maxSize(600, 80);  //设值每个结点的最大长度和宽度

    const QString placeholderText = m_label->placeholderText();
    const QString text = ((dataText.size() == 0) ? placeholderText : dataText);
    const QRect textRect = fm.boundingRect(
                QRect(QPoint(0,0), maxSize),
                Qt::TextWordWrap | Qt::TextWrapAnywhere,
                text);

    const QSize textSizeAdd(
                textRect.width() + addSize.width(),
                textRect.height() + addSize.height() );

    m_size = QSize( qMin(maxSize.width(), textSizeAdd.width()),
                    qMin(maxSize.height(), textSizeAdd.height()));

    resize(m_size);
    emit OnWidgetResize(this, m_size);
}

