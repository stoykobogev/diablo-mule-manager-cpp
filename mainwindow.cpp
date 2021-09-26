#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qstringlistmodel.h"
#include "dataservice.h"
#include "item.h"
#include <iostream>
#include <fstream>
#include <QListWidget>
#include "dialog.h"
#include <QUuid>
#include <QStandardItemModel>
#include <QTableWidget>

const QString MainWindow::NEW_MULE = QString("New...");

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    this->statusBar()->setSizeGripEnabled(false);

    initializeItems();
    initializeMules();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// General

void MainWindow::initializeItems()
{
    ui->chooseItemComboBox->clear();

    ui->itemListWidget->clear();

    for (auto&& item : DataService::ITEMS) {
        ui->itemListWidget->addItem(item.name.c_str());
        ui->chooseItemComboBox->addItem(item.name.c_str());
    }
}

void MainWindow::initializeMules() {
    ui->muleComboBox->clear();
    ui->muleItemsListWidget->clear();

    ui->muleComboBox->addItem(MainWindow::NEW_MULE);
    selectedMule = new Mule;

    for (auto&& mule : DataService::MULES) {
        ui->muleComboBox->addItem(QString(mule.name.c_str()));
    }

    initializeView(DataService::MULES);
}

void MainWindow::initializeView(vector<Mule> &mules) {
    QFont font;
    font.setFamilies({QString::fromUtf8("Calibri")});
    font.setPointSize(12);

    // Mule tree
    if (muleItemTreeViewModel == NULL) {
        muleItemTreeViewModel = new QStandardItemModel(this);
    } else {
        muleItemTreeViewModel->clear();
    }

    ui->muleItemTreeView->reset();

    QStandardItem* headerItem = new QStandardItem("Mules");
    headerItem->setFont(font);
    muleItemTreeViewModel->setHorizontalHeaderItem(0, headerItem);

    QStandardItem* rootItem = muleItemTreeViewModel->invisibleRootItem();
    rootItem->setEditable(false);

    for (auto mule_it = mules.begin(); mule_it != mules.end(); mule_it++) {
        QStandardItem *muleStandardItem = new QStandardItem(mule_it->name.c_str());
        muleStandardItem->setEditable(false);

        for (auto muleItem_it = mule_it->items.begin(); muleItem_it != mule_it->items.end(); muleItem_it++) {
            QStandardItem *muleItemStandardItem = new QStandardItem(muleItem_it->name.c_str());
            muleItemStandardItem->setEditable(false);

            if (!muleItem_it->description.empty()) {
                QStandardItem *descriptionStandardItem = new QStandardItem(("Description: " + muleItem_it->description).c_str());
                descriptionStandardItem->setEditable(false);

                muleItemStandardItem->appendRow(descriptionStandardItem);
            }

            if (!muleItem_it->note.empty()) {
                QStandardItem *noteStandardItem = new QStandardItem(("Note: " + muleItem_it->note).c_str());
                noteStandardItem->setEditable(false);

                muleItemStandardItem->appendRow(noteStandardItem);
            }

            if (muleItem_it->quantity > 1) {
                QStandardItem *quantityStandardItem = new QStandardItem(("Quantity: " + to_string(muleItem_it->quantity)).c_str());
                quantityStandardItem->setEditable(false);

                muleItemStandardItem->appendRow(quantityStandardItem);
            }

            muleStandardItem->appendRow(muleItemStandardItem);
        }

        rootItem->appendRow(muleStandardItem);
    }

    ui->muleItemTreeView->setModel(muleItemTreeViewModel);


    // Item table
    ui->muleItemViewTableWidget->clear();
    ui->muleItemViewTableWidget->model()->removeRows(0, ui->muleItemViewTableWidget->rowCount());

    QTableWidgetItem *column1 = new QTableWidgetItem();
    column1->setText(QString::fromUtf8("Name"));
    column1->setFont(font);
    ui->muleItemViewTableWidget->setHorizontalHeaderItem(0, column1);

    QTableWidgetItem *column2 = new QTableWidgetItem();
    column2->setText(QString::fromUtf8("Quantity"));
    column2->setFont(font);
    ui->muleItemViewTableWidget->setHorizontalHeaderItem(1, column2);

    ui->muleItemViewTableWidget->setColumnWidth(0, 329);
    ui->muleItemViewTableWidget->setColumnWidth(1, 120);

    map<string,int> itemQuantityMap;

    for (auto mule_it = mules.begin(); mule_it != mules.end(); mule_it++) {
        for (auto muleItem_it = mule_it->items.begin(); muleItem_it != mule_it->items.end(); muleItem_it++) {
            if (itemQuantityMap.count(muleItem_it->name)) {
               std::map<string,int>::iterator itemQuantity_it = itemQuantityMap.find(muleItem_it->name);
               itemQuantity_it->second += muleItem_it->quantity;
               itemQuantityMap.insert(std::pair<string,int>(itemQuantity_it->first, itemQuantity_it->second + muleItem_it->quantity));
            } else {
                itemQuantityMap.insert(std::pair<string,int>(muleItem_it->name, muleItem_it->quantity));
            }
        }
    }

    for (auto&& it = itemQuantityMap.begin(); it != itemQuantityMap.end(); it++) {
        int newRowIndex = ui->muleItemViewTableWidget->rowCount();
        ui->muleItemViewTableWidget->insertRow(newRowIndex);
        ui->muleItemViewTableWidget->setItem(newRowIndex, 0, new QTableWidgetItem(it->first.c_str()));
        ui->muleItemViewTableWidget->setItem(newRowIndex, 1, new QTableWidgetItem(to_string(it->second).c_str()));
    }
}

// Mules tab

void MainWindow::on_muleComboBox_textActivated(const QString &qs) {
    if (qs.isEmpty()) return;

    ui->muleItemsListWidget->clear();
    clearEditItemFields();
    clearSelectedMule();

    if (qs == MainWindow::NEW_MULE) {
        selectedMule = new Mule;
    } else {
        string muleName = qs.toStdString();
        selectedMule = DataService::findMule(muleName);
        ui->muleNameLineEdit->setText(qs);

        for (auto&& item : selectedMule->items) {
            ui->muleItemsListWidget->addItem(item.name.c_str());
        }
    }
}

void MainWindow::on_addMuleItemButton_clicked() {
    if (selectedMule != NULL) {
        string itemName = ui->chooseItemComboBox->currentText().toStdString();

        MuleItem muleItem;
        Item* item = DataService::findItem(itemName);
        muleItem.id = item->id;
        muleItem.name = itemName;

        ui->muleItemsListWidget->addItem(muleItem.name.c_str());
        selectedMule->items.push_back(muleItem);
    }
}

void MainWindow::on_removeMuleItemButton_clicked() {
    if(ui->muleItemsListWidget->selectedItems().size() != 0) {
        int rowIndex = ui->muleItemsListWidget->selectionModel()->selectedRows().at(0).row();
        ui->muleItemsListWidget->model()->removeRow(rowIndex);
        selectedMule->items.erase(selectedMule->items.begin() + rowIndex);
    }
}

void MainWindow::on_muleItemsListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous) {
    if (!selectedMule) return;

    if (previous) {
        int indexRow = ui->muleItemsListWidget->indexFromItem(previous).row();
        MuleItem* muleItem = &*(selectedMule->items.begin() + indexRow);
        muleItem->description = ui->editItemDescriptionLineEdit->text().toStdString();
        muleItem->note = ui->editItemNoteLineEdit->text().toStdString();
        muleItem->quantity = ui->editItemQuantityLineEdit->text().toInt();
    }

    if (current) {
        string itemName = current->text().toStdString();
        ui->editItemNameLabel->setText(itemName.c_str());

        int indexRow = ui->muleItemsListWidget->indexFromItem(current).row();
        MuleItem muleItem = selectedMule->items[indexRow];
        ui->editItemDescriptionLineEdit->setText(muleItem.description.c_str());
        ui->editItemNoteLineEdit->setText(muleItem.note.c_str());
        ui->editItemQuantityLineEdit->setText(to_string(muleItem.quantity).c_str());
    } else {
        clearEditItemFields();
    }
}

void MainWindow::on_saveMulesButton_clicked() {

    string muleName = ui->muleNameLineEdit->text().toStdString();
    if (!muleName.empty()) {
        for (int i = 0; i < DataService::MULES.size(); i++) {
            if (muleName == DataService::MULES.at(i).name && muleName != selectedMule->name) {
                ui->statusBar->showMessage("Mule with that name exists", 5000);
                return;
            }
        }
    } else {
        ui->statusBar->showMessage("Mule name is empty", 5000);
        return;
    }

    dialog.setText("Are you sure you want to save mule?");
    connect(&dialog, &Dialog::accepted, this, &MainWindow::saveMules);
    connect(&dialog, &Dialog::rejected, this, &MainWindow::closeSaveMules);
    dialog.show();
}

void MainWindow::saveMules() {
    if (ui->muleItemsListWidget->selectedItems().size() > 0) {
        on_muleItemsListWidget_currentItemChanged(NULL, ui->muleItemsListWidget->selectedItems().at(0));
    }

    selectedMule->name = ui->muleNameLineEdit->text().toStdString();
    if (ui->muleComboBox->currentText() == MainWindow::NEW_MULE) {
        DataService::MULES.push_back(*selectedMule);
    }
    DataService::saveMules();

    clearSelectedMule();
    clearEditItemFields();
    initializeMules();

    ui->statusBar->showMessage("Mules saved", 5000);

    closeSaveMules();
}

void MainWindow::closeSaveMules() {
    disconnect(&dialog, &Dialog::accepted, this, &MainWindow::saveMules);
    disconnect(&dialog, &Dialog::rejected, this, &MainWindow::closeSaveMules);
}

void MainWindow::on_deleteMuleButton_clicked() {
    if (ui->muleComboBox->currentText() != MainWindow::NEW_MULE) {
        dialog.setText("Are you sure you want to delete mule?");
        connect(&dialog, &Dialog::accepted, this, &MainWindow::deleteMule);
        connect(&dialog, &Dialog::rejected, this, &MainWindow::closeDeleteMule);
        dialog.show();
    }
}

void MainWindow::closeDeleteMule() {
    disconnect(&dialog, &Dialog::accepted, this, &MainWindow::deleteMule);
    disconnect(&dialog, &Dialog::rejected, this, &MainWindow::closeDeleteMule);
}

void MainWindow::deleteMule() {
    string muleName = ui->muleComboBox->currentText().toStdString();

    if (muleName != NEW_MULE.toStdString()) {
        for (auto it = DataService::MULES.begin(); it != DataService::MULES.end(); it++) {
            if (it->name == muleName) {
                clearSelectedMule();
                clearEditItemFields();

                DataService::MULES.erase(it);
                DataService::saveMules();
                initializeMules();

                string message = "Mule '" + muleName + "' was deleted";
                ui->statusBar->showMessage(message.c_str(), 5000);
                break;
            }
        }
    }

    closeDeleteMule();
}

void MainWindow::clearEditItemFields() {
    ui->editItemNameLabel->clear();
    ui->editItemDescriptionLineEdit->clear();
    ui->editItemNoteLineEdit->clear();
    ui->editItemQuantityLineEdit->clear();
}

void MainWindow::clearSelectedMule() {
    ui->muleNameLineEdit->clear();

    if (selectedMule != NULL) {
        for (int i = 0; i < DataService::MULES.size(); i++) {
            if (DataService::MULES.at(i).name == selectedMule->name) {
                selectedMule = NULL;
                return;
            }
        }

        delete selectedMule;
        selectedMule = NULL;
    }

}

// Items tab

void MainWindow::on_saveItemsButton_clicked()
{
    dialog.setText("Are you sure you want to save items?\nThis will change them for all mules.");
    connect(&dialog, &Dialog::accepted, this, &MainWindow::saveItems);
    connect(&dialog, &Dialog::rejected, this, &MainWindow::closeSaveItems);
    dialog.show();
}

void MainWindow::saveItems() {
    DataService::saveItems();

    clearSelectedMule();
    clearEditItemFields();
    initializeItems();
    initializeMules();

    ui->statusBar->showMessage("Items saved", 5000);

    closeSaveItems();
}

void MainWindow::closeSaveItems() {
    disconnect(&dialog, &Dialog::accepted, this, &MainWindow::saveItems);
    disconnect(&dialog, &Dialog::rejected, this, &MainWindow::closeSaveItems);
}

void MainWindow::on_deleteItemButton_clicked()
{
    if(ui->itemListWidget->selectedItems().size() != 0) {
        string itemName = ui->itemListWidget->currentItem()->text().toStdString();
        ui->itemListWidget->model()->removeRow(ui->itemListWidget->selectionModel()->selectedRows().at(0).row());

        for (auto it = DataService::ITEMS.begin(); it != DataService::ITEMS.end(); it++) {
            if (itemName == it->name) {
                if (selectedItem == &*it) {
                    selectedItem = NULL;
                }

                DataService::ITEMS.erase(it);
                ui->statusBar->showMessage("Item deleted. Click 'Save' to confirm the changes", 5000);

                break;
            }
        }
    }
}

void MainWindow::on_addItemButton_clicked()
{
    string itemName = ui->newItemLineEdit->text().toStdString();
    if (!itemName.empty()) {
        if (DataService::findItem(itemName)) {
            string message = "Item with name'" + itemName + "' already exists";
            ui->statusBar->showMessage(message.c_str(), 5000);
            return;
        }

        Item item;
        item.id = QUuid::createUuid().toString().toStdString();
        item.name = itemName;
        DataService::ITEMS.push_back(item);

        ui->itemListWidget->addItem(ui->newItemLineEdit->text());
        ui->newItemLineEdit->clear();
        ui->statusBar->showMessage("Item added. Click 'Save' to confirm the changes", 5000);
    }
}

void MainWindow::on_editItemButton_clicked() {
    if (selectedItem) {
        selectedItem->name = ui->editItemLineEdit->text().toStdString();
        ui->itemListWidget->selectedItems().at(0)->setText(selectedItem->name.c_str());
        ui->statusBar->showMessage("Item edited. Click 'Save' to confirm the changes", 5000);
    }
}

void MainWindow::on_itemListWidget_currentTextChanged(const QString &currentText) {

    if (!currentText.isNull() && !currentText.isEmpty()) {
        ui->editItemLineEdit->setText(currentText);

        string itemName = currentText.toStdString();
        Item* item = DataService::findItem(itemName);

        if (item) {
            selectedItem = item;
        }
    } else {
        ui->editItemLineEdit->clear();
    }
}

// View tab

void MainWindow::on_searchItemButton_clicked() {
    if (ui->searchItemLineEdit->text().isEmpty()) return;

    vector<Mule> mules;
    string itemName = ui->searchItemLineEdit->text().toStdString();

    for (auto&& mule_it = DataService::MULES.begin(); mule_it != DataService::MULES.end(); mule_it++) {
        Mule mule;
        for (auto muleItem_it = mule_it->items.begin(); muleItem_it != mule_it->items.end(); muleItem_it++) {
            if (icompare(muleItem_it->name, itemName)) {
                mule.items.push_back(*muleItem_it);
                break;
            }
        }

        if (mule.items.size() > 0) {
            mule.name = mule_it->name;
            mules.push_back(mule);
        }
    }

    initializeView(mules);
}

bool MainWindow::icompare_pred(unsigned char a, unsigned char b)
{
    return std::tolower(a) == std::tolower(b);
}

bool MainWindow::icompare(std::string const& a, std::string const& b)
{
    auto it = std::search(
        a.begin(), a.end(),
        b.begin(), b.end(),
        icompare_pred
      );
    return (it != a.end() );
}

void MainWindow::on_clearSearchItemButton_clicked() {
    ui->searchItemLineEdit->clear();
    initializeView(DataService::MULES);
}
