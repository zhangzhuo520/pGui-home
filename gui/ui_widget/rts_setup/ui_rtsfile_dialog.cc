#include "ui_rtsfile_dialog.h"
namespace ui {

RtsFileDialog::RtsFileDialog(QWidget *parent):
    QDialog(parent)
{
    init_ui();

    init_filedialog();

    init_connection();
}

RtsFileDialog::~RtsFileDialog()
{
    delete m_file_dialog;
    m_file_dialog = NULL;
}

void RtsFileDialog::init_ui()
{
    m_rts_fileedit = new RtsFileEdit(this);
    m_open_button = new QPushButton("Open",this);
    m_save_button = new QPushButton("Save", this);
    QHBoxLayout *hlayout = new QHBoxLayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    hlayout->setSpacing(2);
    hlayout->setContentsMargins(0, 0, 0, 0);
    vlayout->addWidget(m_open_button);
    vlayout->addWidget(m_save_button);
    vlayout->addWidget(new QLabel(""));
    vlayout->setSpacing(5);
    vlayout->setStretch(0, 1);
    vlayout->setStretch(1, 1);
    vlayout->setStretch(2, 8);

    hlayout->addWidget(m_rts_fileedit);
    hlayout->addLayout(vlayout);
    setLayout(hlayout);

    m_rts_fileedit->set_mode(RtsFileEdit::EDIT);
    HighLighter *highlighter = new HighLighter(m_rts_fileedit->document());
    Q_UNUSED(highlighter);

    QFont font;
    font.setFamily("Sans Serif");
    font.setPointSize (16);
    m_rts_fileedit->setFont(font);
}

void RtsFileDialog::init_filedialog()
{
    m_file_path = QDir::homePath() + "/.pangen_gui/pgui_rts";
    m_file_dialog = new QFileDialog(this);
    m_file_dialog->setWindowModality(Qt::ApplicationModal);
    m_file_dialog->setWindowTitle(tr("Open Rts File"));
    m_file_dialog->setNameFilter(tr("Files(*.py *.sh)"));
    connect(m_file_dialog, SIGNAL(fileSelected(QString)), this, SLOT(slot_get_file(QString)), Qt::UniqueConnection);
    m_file_dialog->setFileMode(QFileDialog::ExistingFiles);
    m_file_dialog->setViewMode(QFileDialog::List);
}

void RtsFileDialog::init_connection()
{
    connect(m_open_button, SIGNAL(clicked()), this, SLOT(slot_open_file()));
    connect(m_save_button, SIGNAL(clicked()), this, SLOT(slot_save_file()));
    connect(m_rts_fileedit, SIGNAL(textChanged()), SLOT(slot_text_change()));
}

void RtsFileDialog::slot_save_file()
{
    QFile file(m_current_file);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        logger_widget("file path error!");
        return;
    }
    QString data = m_rts_fileedit->document()->toPlainText();
    QTextStream out(&file);
    out << data;
    out.flush();
    file.close();
    setWindowTitle(m_current_file);
}

void RtsFileDialog::slot_open_file()
{
    m_file_dialog->setDirectory(m_file_path);
    m_file_dialog->show();
}

void RtsFileDialog::slot_get_file(QString filename)
{
    m_current_file = filename;
    QString file_data = read_file(filename);
    m_rts_fileedit->clear();
    m_rts_fileedit->appendPlainText(file_data);
    setWindowTitle(filename);
}

QString RtsFileDialog::read_file(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        logger_widget("rts file not exist!");
        return QString();
    }
    QString file_data = file.readAll();
    file.flush();
    file.close();
    return file_data;
}

void RtsFileDialog::slot_text_change()
{
    setWindowTitle(m_current_file + "*");
}
}
