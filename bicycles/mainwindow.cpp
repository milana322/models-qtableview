#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tableView = new QTableView(this);
    catsCheckBox = new QCheckBox("Show Cats", this);
    bikesCheckBox = new QCheckBox("Show Bikes", this);
    saveButton = new QPushButton("Save", this);
    addButton = new QPushButton("Add Row", this);
    sortByNameButton = new QPushButton("Sort by Name", this);
    findButton = new QPushButton("Find", this);

    layout = new QVBoxLayout;
    layout->addWidget(catsCheckBox);
    layout->addWidget(bikesCheckBox);
    layout->addWidget(tableView);
    layout->addWidget(saveButton);
    layout->addWidget(addButton);
    layout->addWidget(sortByNameButton);
    layout->addWidget(findButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    setupCatModel();
    setupBikeModel();

    connect(catsCheckBox, &QCheckBox::stateChanged, this, &MainWindow::on_catsCheckBox_stateChanged);
    connect(bikesCheckBox, &QCheckBox::stateChanged, this, &MainWindow::on_bikesCheckBox_stateChanged);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::onSaveButtonClicked);
    connect(addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(sortByNameButton, &QPushButton::clicked, this, &MainWindow::onSortByNameButtonClicked);
    connect(findButton, &QPushButton::clicked, this, &MainWindow::onFindButtonClicked);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupCatModel() {
    catModel = new QStandardItemModel(0, 4, this);  // создается модель без строк с данными
    catModel->setHorizontalHeaderLabels({"Name", "Breed", "Weight", "Origin"});
    readFromFile("cats.txt", catModel, true);
}

void MainWindow::setupBikeModel() {
    bikeModel = new QStandardItemModel(0, 5, this);
    bikeModel->setHorizontalHeaderLabels({"Brand", "Color", "Rating", "Price", "Year"});
    readFromFile("bikes.txt", bikeModel, false);
}

void MainWindow::readFromFile(const QString &filename, QStandardItemModel *model, bool isCat) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("Cannot open file for reading");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QList<QStandardItem*> items;
        if (isCat) {
            Cats cat;
            in >> cat;
            items.append(new QStandardItem(cat.Name));
            items.append(new QStandardItem(cat.Breed));
            items.append(new QStandardItem(cat.Weight));
            items.append(new QStandardItem(cat.Origin));
        } else {
            Bikes bike;
            in >> bike;
            items.append(new QStandardItem(bike.Name));
            items.append(new QStandardItem(bike.Color));
            items.append(new QStandardItem(bike.Rating));
            items.append(new QStandardItem(QString::number(bike.Price)));
            items.append(new QStandardItem(QString::number(bike.Year)));
        }
        model->appendRow(items);
    }
    file.close();
}

void MainWindow::writeToFile(const QString &filename, QStandardItemModel *model) {
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning("Cannot open file for writing");
        return;
    }

    QTextStream out(&file);
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            out << model->item(row, col)->text() << "\n";
        }
    }
    file.close();
}

void MainWindow::on_catsCheckBox_stateChanged(int state) {
    if (state == Qt::Checked) {
        bikesCheckBox->setChecked(false);
        tableView->setModel(catModel);
    } else if (state == Qt::Unchecked) {
        tableView->setModel(nullptr);  // очистить таблицу
    }
}

void MainWindow::on_bikesCheckBox_stateChanged(int state) {
    if (state == Qt::Checked) {
        catsCheckBox->setChecked(false);
        tableView->setModel(bikeModel);
    } else if (state == Qt::Unchecked) {
        tableView->setModel(nullptr);
    }
}

void MainWindow::onSaveButtonClicked() {//записать в файл таблицу
    if (catsCheckBox->isChecked()) {
        writeToFile("cats.txt", catModel);
    } else if (bikesCheckBox->isChecked()) {
        writeToFile("bikes.txt", bikeModel);
    }
}

void MainWindow::onAddButtonClicked() {
    if (catsCheckBox->isChecked()) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem("New Name"));//данные о новом объекте по умолчанию
        items.append(new QStandardItem("New Breed"));
        items.append(new QStandardItem("New Weight"));
        items.append(new QStandardItem("New Origin"));
        catModel->appendRow(items);
    } else if (bikesCheckBox->isChecked()) {
        QList<QStandardItem*> items;
        items.append(new QStandardItem("New Brand"));
        items.append(new QStandardItem("New Color"));
        items.append(new QStandardItem("New Rating"));
        items.append(new QStandardItem("0.0"));
        items.append(new QStandardItem("2000"));
        bikeModel->appendRow(items);
    }
}



template<typename T>
void MainWindow::sortByName(QStandardItemModel *model) {
    QVector<QPair<QStringList, int>> rows;
    for (int row = 0; row < model->rowCount(); ++row) {
        QStringList data;
        for (int col = 0; col < model->columnCount(); ++col) {
            data.append(model->item(row, col)->text());
        }
        rows.append(qMakePair(data, row));
    }

    std::sort(rows.begin(), rows.end(), [](const QPair<QStringList, int>& left, const QPair<QStringList, int>& right) {
        return left.first.at(0) < right.first.at(0); // сортировка по имени
    });

    for (int row = 0; row < rows.size(); ++row) {
        for (int col = 0; col < model->columnCount(); ++col) {
            model->setItem(row, col, new QStandardItem(rows[row].first.at(col)));
        }
    }
}

template<typename T>
bool MainWindow::findByName(const QString &name, QStandardItemModel *model) {
    QModelIndexList matches = model->match(model->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchExactly | Qt::MatchCaseSensitive);//метод, ищущий все совпадения в модели по колонке имен
    return !matches.isEmpty();
}

void MainWindow::onSortByNameButtonClicked() {
    if (catsCheckBox->isChecked()) {
        sortByName<Cats>(catModel);
    } else if (bikesCheckBox->isChecked()) {
        sortByName<Bikes>(bikeModel);
    }
}

void MainWindow::onFindButtonClicked() {
    QString name = QInputDialog::getText(this, "Find", "Enter name to search:");
    if (catsCheckBox->isChecked()) {
        bool found = findByName<Cats>(name, catModel);
        if (found) {
            QMessageBox::information(this, "Find Result", "Found in cats table.");
        } else {
            QMessageBox::information(this, "Find Result", "Not found in cats table.");
        }
    } else if (bikesCheckBox->isChecked()) {
        bool found = findByName<Bikes>(name, bikeModel);
        if (found) {
            QMessageBox::information(this, "Find Result", "Found in bikes table.");
        } else {
            QMessageBox::information(this, "Find Result", "Not found in bikes table.");
        }
    }
}
