#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QTableView>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>


struct Cats {
    QString Name, Breed, Weight, Origin;

    friend QTextStream& operator>>(QTextStream& in, Cats& cat) {
        cat.Name = in.readLine();
        cat.Breed = in.readLine();
        cat.Weight = in.readLine();
        cat.Origin = in.readLine();
        return in;
    }

    friend QTextStream& operator<<(QTextStream& out, const Cats& cat) {
        out << cat.Name << "\n" << cat.Breed << "\n" << cat.Weight << "\n" << cat.Origin << "\n";
        return out;
    }

    bool operator==(const Cats& other) const {
        return Name == other.Name;
    }
};

struct Bikes {
    QString Name, Color, Rating;
    double Price;
    int Year;

    friend QTextStream& operator>>(QTextStream& in, Bikes& bike) {
        bike.Name = in.readLine();
        bike.Color = in.readLine();
        bike.Rating = in.readLine();
        in >> bike.Price;
        in.readLine();
        in >> bike.Year;
        in.readLine();
        return in;
    }

    friend QTextStream& operator<<(QTextStream& out, const Bikes& bike) {
        out << bike.Name << "\n" << bike.Color << "\n" << bike.Rating << "\n" << bike.Price << "\n" << bike.Year << "\n";
        return out;
    }

    bool operator==(const Bikes& other) const {
        return Name == other.Name;
    }
};
// template<class T>
// class ArraySorter {
//     QVector<T> items;
// public:
//     void sortByName() {
//         std::sort(items.begin(), items.end(), [](const T& a, const T& b) {
//             return qstrcmp(a.Name, b.Name) > 0;
//         });
//     }

// };

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
    void on_catsCheckBox_stateChanged(int state);
    void on_bikesCheckBox_stateChanged(int state);
    void onSaveButtonClicked();
    void onAddButtonClicked();
    void onSortByNameButtonClicked();
    void onFindButtonClicked();

private:

    void setupCatModel();
    void setupBikeModel();
    void readFromFile(const QString &filename, QStandardItemModel *model, bool isCat);
    void writeToFile(const QString &filename, QStandardItemModel *model);

    template<typename T>
    static void sortByName(QStandardItemModel *model);

    template<typename T>
    static bool findByName(const QString &name, QStandardItemModel *model);

    Ui::MainWindow *ui;
    QTableView *tableView;
    QCheckBox *catsCheckBox;
    QCheckBox *bikesCheckBox;
    QVBoxLayout *layout;
    QPushButton *saveButton;
    QPushButton *addButton;
    QPushButton *sortByNameButton;
    QPushButton *findButton;
    QStandardItemModel *catModel;
    QStandardItemModel *bikeModel;
};

#endif // MAINWINDOW_H



