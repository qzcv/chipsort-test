#ifndef SURFACEPADVANCEDIALOG_H
#define SURFACEPADVANCEDIALOG_H

#include <QDialog>

namespace Ui {
class surfacePAdvanceDialog;
}

class surfacePAdvanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit surfacePAdvanceDialog(QWidget *parent = 0);
    ~surfacePAdvanceDialog();
	Ui::surfacePAdvanceDialog *ui;

private:
};

#endif // SURFACEPADVANCEDIALOG_H
