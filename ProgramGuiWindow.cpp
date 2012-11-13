#include <QHBoxLayout>
#include <QVBoxLayout>
#include "qextserialport.h"
#include "qextserialenumerator.h"

#include "ProgramGuiWindow.h"

ProgramGuiWindow::ProgramGuiWindow(QWidget* parent) :
    QWidget(parent)
{
    // create the controls
    m_texteditProgram = new QTextEdit();
    //m_texteditProgram->setText(
    m_texteditProgram->insertPlainText(
            "change channel 1 to repeat 15 ms on 85 ms off\n"
            "wait 3 s\n"
            "turn off channel 1\n"
            "end\n"
            );
    /*
     * m_texteditProgram->setTextColor(QColor(255, 0, 0));
    m_texteditProgram->insertPlainText(
            "end?\n"
            );
    m_texteditProgram->setTextColor(QColor(0, 0, 0));
    m_texteditProgram->insertPlainText(
            "end!\n"
            );
            */
    m_texteditProgram->setLineWrapMode(QTextEdit::NoWrap);

    m_texteditStatus = new QTextEdit();
    m_texteditStatus->setReadOnly(true);

    m_comboPort = new QComboBox();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        m_comboPort->addItem(info.portName);
    m_comboPort->setEditable(true);
    int index_ttyACM0 = m_comboPort->findText("ttyACM0");
    if (index_ttyACM0 > -1) {
        m_comboPort->setCurrentIndex(index_ttyACM0);
    }


    m_buttonLoad = new QPushButton("Load");
    m_buttonSave = new QPushButton("Save");
    m_buttonRun = new QPushButton("Run");


    // lay out the controls
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_comboPort);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_buttonLoad);
    buttonLayout->addWidget(m_buttonSave);
    buttonLayout->addWidget(m_buttonRun);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_texteditProgram);
    mainLayout->setStretch(0,4);
    //mainLayout->addWidget(m_labelStatus);
    mainLayout->addWidget(m_texteditStatus);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // attach signals as needed
    QObject::connect(m_buttonRun, SIGNAL(clicked()), this, SLOT(run()));
    QObject::connect(m_comboPort, SIGNAL(editTextChanged(QString)), SLOT(onPortChanged()));

    m_port = NULL;
    onPortChanged();
};


QSize ProgramGuiWindow::sizeHint() const {
    return QSize(600,600);
}


void ProgramGuiWindow::run() {
    m_port->write(m_texteditProgram->toPlainText().toUtf8());
}

void ProgramGuiWindow::onNewSerialData() {
    if (m_port->bytesAvailable()) {
        m_texteditStatus->moveCursor(QTextCursor::End);
        m_texteditStatus->insertPlainText(QString::fromUtf8(m_port->readAll()).replace("\n",""));
    }
}

void ProgramGuiWindow::onPortChanged() {
    if (m_port) {
        m_port->close();
        delete m_port;
    }

    // create the serial port
    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    m_port = new QextSerialPort(m_comboPort->currentText(), settings, QextSerialPort::EventDriven);
    QObject::connect(m_port, SIGNAL(readyRead()), SLOT(onNewSerialData()));
    m_port->open(QIODevice::ReadWrite);
}