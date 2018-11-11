#include "fileprojectwidget.h"

FileProjectWidget::FileProjectWidget(QWidget *parent) :
    QWidget(parent)
{
    init();
    QVBoxLayout *Vayout = new QVBoxLayout(this);
    Vayout->addWidget(projectTable);
    Vayout->setContentsMargins(0, 0, 0, 0);
    Vayout->setSpacing(0);
    setLayout(Vayout);
}

void FileProjectWidget::init()
{
    projectTable = new QTableView(this);
    projectTableModel = new QStandardItemModel(projectTable);

    projectTable->setModel(projectTableModel);
    projectTableModel->setHorizontalHeaderLabels (QStringList() << "filename");
    projectTable->horizontalHeader()->setClickable(false);
    projectTable->horizontalHeader()->setStretchLastSection(true);
    projectTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

    projectTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    projectTable->setSelectionMode(QAbstractItemView::SingleSelection);
    projectTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    projectTable->verticalHeader()->hide();

    connect(projectTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slot_click_fileItem(QModelIndex)));
}

void FileProjectWidget::slot_click_fileItem(QModelIndex index)
{
    emit signal_click_fileItem(index);
}

void FileProjectWidget::slot_addFile(QString path)
{
#if 1 //check many file

        QStringList filenameList = path.split('/');
        projectTableModel->appendRow(new QStandardItem(filenameList.at(filenameList.count() - 1)));

    for (int i = 0; i < projectTableModel->rowCount(); i ++)
    {
        projectTableModel->item(i)->setTextAlignment(Qt::AlignCenter);
    }
    QModelIndex modelIndex = projectTableModel->index(projectTableModel->rowCount() - 1, 0);
    emit signal_click_fileItem(modelIndex);
#else
    // check only one
    QStringList filenameList = path.split('/');
    projectTableModel->setItem(i,new QStandardItem(filenameList.at(filenameList.count() - 1)));
    projectTableModel->item(i)->setTextAlignment(Qt::AlignCenter);
    projectTable->show();
#endif
}
