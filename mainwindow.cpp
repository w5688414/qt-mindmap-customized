#include "mainwindow.h"
//#include "ui_mainwindow.h"
#include "mapcontextwidget.h"
#include "mapcontext.h"

#include <QGridLayout>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

const QString TEST_MUPNAME = "/Users/eric/Documents/qt_projects/mindmap.mup"; //json like files 文件路径
const QString APP_NAME = "Demo";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)

{
    setWindowTitle(APP_NAME);
    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget( centralWidget );
    QGridLayout* layout = new QGridLayout( centralWidget );
    layout->setMargin(0);
    MindMap::MapContext* m_context = NULL;
    {
        QFile loadFile(TEST_MUPNAME);
        if(loadFile.open(QIODevice::ReadOnly)){
            QByteArray dataBytes = loadFile.readAll();
            QJsonDocument loadDoc = QJsonDocument::fromJson(dataBytes);
            m_context = new MindMap::MapContext(loadDoc.object()["ideas"].toObject()["1"].toObject());

        }else{
            qWarning("cannot open input file");
        }
    }
    if (m_context == NULL){
        m_context = new MindMap::MapContext;
    }

    m_mainWidget = new MapContextWidget(centralWidget, m_context);
    layout->addWidget(m_mainWidget, 0, 0);

    resize(800, 800);


}

MainWindow::~MainWindow()
{

}
