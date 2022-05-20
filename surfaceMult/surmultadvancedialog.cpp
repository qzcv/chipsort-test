#include "surmultadvancedialog.h"
#include "ui_surmultadvancedialog.h"

surMultAdvanceDialog::surMultAdvanceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::surMultAdvanceDialog)
{
    ui->setupUi(this);
}

surMultAdvanceDialog::~surMultAdvanceDialog()
{
    delete ui;
}
