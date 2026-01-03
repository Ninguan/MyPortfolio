#include "mainwindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QMessageBox>
#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    m_dataFilePath = QCoreApplication::applicationDirPath() + "/zadanie.txt";
    m_inputTable = new QTableWidget(this);
    m_inputTable->setRowCount(1);
    m_inputTable->setColumnCount(3);
    QStringList headers;
    headers << "Nazwisko" << "Imie" << "Wiek";
    m_inputTable->setHorizontalHeaderLabels(headers);
    m_inputTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_inputTable->verticalHeader()->setVisible(false);
    m_inputTable->setItem(0, 0, new QTableWidgetItem());
    m_inputTable->setItem(0, 1, new QTableWidgetItem());
    m_inputTable->setItem(0, 2, new QTableWidgetItem());
    m_searchEdit =new QLineEdit(this);
    m_searchEdit->setPlaceholderText("");
    m_addButton =new QPushButton("dodac", this);
    m_showButton =new QPushButton("wyswietlic wszystkich", this);
    m_listWidget = new QListWidget(this);
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);
    auto *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_addButton);
    buttonsLayout->addWidget(m_showButton);
    mainLayout->addWidget(new QLabel("Input: Nazwicko Imie Wiek: "));
    mainLayout->addWidget(m_inputTable);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(new QLabel("Wyszukiwanie:"));
    mainLayout->addWidget(m_searchEdit);
    mainLayout->addWidget(m_listWidget);
    setCentralWidget(central);
    setWindowTitle("Lista");
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddClicked);
    connect(m_showButton, &QPushButton::clicked, this, &MainWindow::onShowAllClicked);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    loadFromFile();
}

void MainWindow::onAddClicked()
{
    QString surname;
    QString name;
    QString ageStr;
    if (m_inputTable->item(0,0)){
        surname = m_inputTable->item(0,0)->text().trimmed();
    }
    if (m_inputTable->item(0,1)){
        name = m_inputTable->item(0,1)->text().trimmed();
    }
    if (m_inputTable->item(0,2)){
        ageStr = m_inputTable->item(0,2)->text().trimmed();
    }

    if ((surname.isEmpty()) or (name.isEmpty()) or (ageStr.isEmpty())) {
        QMessageBox::warning(this, "Error", "nie wpisane wszystkie dane");
        return;
    }
    bool flag = false;
    int age = ageStr.toInt(&flag);
    if ((!flag) or (age <= 0)) {
        QMessageBox::warning(this, "Error", "wiek powinien byc dodatnim");
        return;
    }
    Person p { surname, name, age };
    if (m_edtIndx == -1){
        m_people.push_back(p);
    }
    else{
        m_people[m_edtIndx] = p;
        m_edtIndx = -1;
        m_addButton->setText("Dodac");
    }
    saveToFile();

    m_inputTable->item(0,0)->setText("");
    m_inputTable->item(0,1)->setText("");
    m_inputTable->item(0,2)->setText("");

}

void MainWindow::onShowAllClicked()
{
    refreshList();
}

void MainWindow::onSearchTextChanged(const QString &text)
{
    QString key = text.trimmed();
    if (key.isEmpty()) {
        m_listWidget->clear();
        return;
    }
    refreshList(key);
}

void MainWindow::onListItDoblClicked(QListWidgetItem *item)
{
    if(!item){
        return;
    }
    int index = item->data(Qt::UserRole).toInt();
    if(index<0 or index>=m_people.size()){
        return;
    }
    m_edtIndx = index;
    const Person &p = m_people[index];
    m_inputTable->item(0,0)->setText(p.surname);
    m_inputTable->item(0,1)->setText(p.name);
    m_inputTable->item(0,2)->setText(QString::number(p.age));
    m_addButton->setText("Zapisac zmiany");
}
void MainWindow::refreshList(const QString &filter)
{
    m_listWidget->clear();
    QString key = filter.trimmed();
    bool useFilter = !key.isEmpty();
    for (int i = 0; i < m_people.size(); ++i) {
        const Person &p = m_people[i];
        if (useFilter) {
            if ((!p.name.contains(key, Qt::CaseInsensitive)) and (!p.surname.contains(key, Qt::CaseInsensitive))) {
                continue;
            }
        }
        QString line = QString("%1 %2, wiek: %3").arg(p.surname).arg(p.name).arg(p.age);
        auto *listItem = new QListWidgetItem(line, m_listWidget);
        listItem->setData(Qt::UserRole, i);
    }
}

void MainWindow::loadFromFile()
{
    m_people.clear();
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        const QString line = in.readLine().trimmed();
        if (line.isEmpty()){
            continue;
        }
        const QStringList parts = line.split(';');
        if (parts.size() != 3){
            continue;
        }
        bool flag = false;
        int age = parts[2].toInt(&flag);
        if (!flag){
            continue;
        }
        Person p { parts[0], parts[1], age };
        m_people.push_back(p);
    }

    file.close();
}

void MainWindow::saveToFile() const
{
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "Error", "Nie udalo sie otworzyc plik");
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    for (const Person &p : m_people) {
        out<<p.surname<<';'<<p.name<<';'<<p.age<<'\n';
    }
    file.close();
}
