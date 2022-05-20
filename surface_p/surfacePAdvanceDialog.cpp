#include "surfacepadvancedialog.h"
#include "ui_surfacepadvancedialog.h"

surfacePAdvanceDialog::surfacePAdvanceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::surfacePAdvanceDialog)
{
    ui->setupUi(this);
}

surfacePAdvanceDialog::~surfacePAdvanceDialog()
{
    delete ui;
}
