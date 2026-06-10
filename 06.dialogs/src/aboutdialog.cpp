#include "aboutdialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

AboutDialog::AboutDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AboutDialog::setupUi()
{
    setWindowTitle(tr("About Demo 05"));
    setMinimumSize(320, 200);

    QVBoxLayout *layout = new QVBoxLayout();
    setLayout(layout);

    QLabel *titleLabel = new QLabel(tr("<h2>Demo 05 - Dialogs</h2>"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QLabel *versionLabel = new QLabel(tr("Version: 1.0.0"), this);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);

    QLabel *descLabel = new QLabel(
        tr("A demonstration of various Qt dialogs:\n"
           "QFileDialog, QColorDialog, QInputDialog,\n"
           "QMessageBox, and custom QDialog."),
        this);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);

    layout->addSpacing(10);

    QLabel *authorLabel = new QLabel(tr("Author: Qt Learner"), this);
    authorLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(authorLabel);

    layout->addSpacing(10);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok, this);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
}
