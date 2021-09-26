#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qstringlistmodel.h"
#include "mule.h"
#include <QListWidgetItem>
#include "dialog.h"
#include <QStandardItemModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_saveItemsButton_clicked();
    void on_deleteItemButton_clicked();
    void on_addItemButton_clicked();
    void on_muleComboBox_textActivated(const QString &qs);
    void on_addMuleItemButton_clicked();
    void on_removeMuleItemButton_clicked();
    void on_muleItemsListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_saveMulesButton_clicked();
    void on_deleteMuleButton_clicked();
    void on_editItemButton_clicked();
    void on_itemListWidget_currentTextChanged(const QString &currentText);
    void on_searchItemButton_clicked();
    void on_clearSearchItemButton_clicked();

private:
    const static QString NEW_MULE;

    Ui::MainWindow *ui;
    Item *selectedItem = NULL;
    Mule *selectedMule = NULL;
    Dialog dialog;
    QStandardItemModel* muleItemTreeViewModel = NULL;

    void initializeItems();
    void initializeMules();
    void initializeView(vector<Mule> &mules);
    void clearEditItemFields();
    void clearSelectedMule();
    void deleteMule();
    void closeDeleteMule();
    void saveItems();
    void closeSaveItems();
    void saveMules();
    void closeSaveMules();
};
#endif // MAINWINDOW_H
