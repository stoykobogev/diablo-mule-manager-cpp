#include "dialog.h"

Dialog::Dialog(QWidget *parent) : QDialog(parent)
{
    QFont font;
    font.setFamilies({QString::fromUtf8("Calibri")});
    font.setPointSize(12);
    font.setBold(false);

    setParent(parent);
    setModal(true);
    setFixedSize(400, 200);
    setWindowTitle("Popup");

    okButton.setParent(this);
    okButton.setGeometry(70, 130, 90, 30);
    okButton.setText("OK");
    okButton.setFont(font);

    connect(&okButton, &QPushButton::clicked, this, &Dialog::accept);

    cancelButton.setParent(this);
    cancelButton.setGeometry(240, 130, 90, 30);
    cancelButton.setText("Cancel");
    cancelButton.setFont(font);

    connect(&cancelButton, &QPushButton::clicked, this, &Dialog::reject);

    label.setParent(this);
    label.setGeometry(0, 0, 400, 120);
    label.setFont(font);
    label.setAlignment(Qt::AlignCenter);
    label.setWordWrap(true);
}

void Dialog::setText(std::string text) {
    label.setText(text.c_str());
}
