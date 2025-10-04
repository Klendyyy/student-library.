#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE
class QTableView;
class QPushButton;
class QTabWidget;
class QSqlTableModel;
class QSqlQueryModel;
class QLabel;
class QMenu;
class QAction;
class QToolButton;
class QComboBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(const QSqlRecord &userRecord, QWidget *parent = nullptr);

private slots:
    void addBook();
    void editBook();
    void deleteBook();
    void rentBook();
    void returnBook();
    void refreshModels();
    void showProfileDialog();
    void logout();
    void updateWelcomeLabel();
    void changeTheme(int index);  // Новый слот для смены темы

private:
    void setupUI();
    void setupAdminTab();
    void setupStudentTab();
    void setupTeacherTab();
    void setupStudentRentalsTab();
    void setupSettingsTab();  // Новый метод для вкладки настроек
    void applyTheme(const QString& theme);  // Метод применения темы

    QSqlTableModel *booksModel = nullptr;
    QSqlQueryModel *availableBooksModel = nullptr;
    QSqlQueryModel *rentalsModel = nullptr;
    QSqlQueryModel *studentRentalsModel = nullptr;
    QTabWidget *tabs = nullptr;
    QSqlRecord currentUser;
    QTableView *adminBooksView = nullptr;
    QTableView *studentBooksView = nullptr;
    QTableView *teacherRentalsView = nullptr;
    QTableView *studentRentalsView = nullptr;
    QLabel *welcomeLabel = nullptr;
    QToolButton *profileButton = nullptr;
    QMenu *profileMenu = nullptr;
    QComboBox *themeComboBox = nullptr;  // Комбобокс для выбора темы
};

#endif
