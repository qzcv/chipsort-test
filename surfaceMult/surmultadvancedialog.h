#ifndef SURMULTADVANCEDIALOG_H
#define SURMULTADVANCEDIALOG_H

#include <QDialog>

namespace Ui {
class surMultAdvanceDialog;
}

class surMultAdvanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit surMultAdvanceDialog(QWidget *parent = 0);
    ~surMultAdvanceDialog();

	Ui::surMultAdvanceDialog *ui;

private:
    
};

#endif // SURMULTADVANCEDIALOG_H
