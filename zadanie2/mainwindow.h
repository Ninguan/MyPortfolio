#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>

struct Person
{
    QString surname;
    QString name;
    int age;
};

class QLineEdit;
class QPushButton;
class QListWidget;
class QTableWidget;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onAddClicked();
    void onShowAllClicked();
    void onSearchTextChanged(const QString &text);
    void onListItDoblClicked(QListWidgetItem *item);

private:
    void loadFromFile();
    void saveToFile() const;
    void refreshList(const QString &filter = QString());

    QTableWidget *m_inputTable;

    QLineEdit *m_searchEdit;
    QPushButton *m_addButton;
    QPushButton *m_showButton;
    QListWidget *m_listWidget;

    QVector<Person> m_people;
    QString m_dataFilePath;

    int m_edtIndx;
};

#endif
