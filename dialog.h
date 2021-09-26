#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>

class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = nullptr);
    void setText(std::string text);

private:
    QPushButton okButton;
    QPushButton cancelButton;
    QLabel label;


};

#endif // DIALOG_H
