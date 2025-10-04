#include "MainWindow.h"
#include "ProfileDialog.h"
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QLabel>
#include <QGroupBox>
#include <QInputDialog>
#include <QDate>
#include <QDebug>
#include <QLineEdit>
#include <QFormLayout>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolButton>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDialog>
#include <QDialogButtonBox>
#include <QComboBox>  // Для комбобокса выбора темы
#include <QSettings>  // Для сохранения настроек

MainWindow::MainWindow(const QSqlRecord &userRecord, QWidget *parent)
    : QMainWindow(parent), currentUser(userRecord) {
    setWindowTitle("Система управления библиотекой");
    setMinimumSize(1000, 700);

    setupUI();
}

void MainWindow::setupUI() {
    // Создаем верхнюю панель с приветствием и меню профиля
    QWidget *headerWidget = new QWidget;
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    welcomeLabel = new QLabel;
    updateWelcomeLabel();

    // Создаем кнопку профиля с выпадающим меню
    profileButton = new QToolButton;
    profileButton->setText("Профиль ▼");
    profileButton->setPopupMode(QToolButton::InstantPopup);
    profileButton->setStyleSheet(
        "QToolButton {"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    border-radius: 5px;"
        "    padding: 8px 15px;"
        "    font-weight: bold;"
        "}"
        "QToolButton:hover {"
        "    background-color: #2980b9;"
        "}"
        "QToolButton::menu-indicator {"
        "    image: none;"
        "}"
        );

    profileMenu = new QMenu(this);
    QAction *editProfileAction = new QAction("Редактировать профиль", this);
    QAction *logoutAction = new QAction("Выйти из аккаунта", this);

    profileMenu->addAction(editProfileAction);
    profileMenu->addSeparator();
    profileMenu->addAction(logoutAction);

    profileButton->setMenu(profileMenu);

    // Добавляем элементы в заголовок
    headerLayout->addWidget(welcomeLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(profileButton);

    tabs = new QTabWidget;

    QString userRole = currentUser.value("role").toString();

    if (userRole == "admin") {
        setupAdminTab();
        setupTeacherTab();
    }
    if (userRole == "teacher") {
        setupTeacherTab();
    }
    if (userRole == "student" || userRole == "admin") {
        setupStudentTab();
    }
    if (userRole == "student") {
        setupStudentRentalsTab();
    }

    // Вкладка настроек доступна всем пользователям
    setupSettingsTab();

    QWidget *centralWidget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(tabs);

    setCentralWidget(centralWidget);

    // Подключаем сигналы меню
    connect(editProfileAction, &QAction::triggered, this, &MainWindow::showProfileDialog);
    connect(logoutAction, &QAction::triggered, this, &MainWindow::logout);

    // Загружаем сохраненную тему
    QSettings settings;
    QString savedTheme = settings.value("theme", "Light").toString();
    applyTheme(savedTheme);

    // Устанавливаем выбранную тему в комбобокс
    if (themeComboBox) {
        if (savedTheme == "Dark") {
            themeComboBox->setCurrentIndex(1);
        } else {
            themeComboBox->setCurrentIndex(0);
        }
    }
}

void MainWindow::setupSettingsTab() {
    QWidget *settingsTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(settingsTab);

    // Группа настроек внешнего вида
    QGroupBox *appearanceGroup = new QGroupBox("Настройки внешнего вида");
    QFormLayout *appearanceLayout = new QFormLayout(appearanceGroup);

    // Выбор темы
    themeComboBox = new QComboBox;
    themeComboBox->addItem("Светлая тема", "Light");
    themeComboBox->addItem("Темная тема", "Dark");

    appearanceLayout->addRow("Тема оформления:", themeComboBox);

    // Информационная группа
    QGroupBox *infoGroup = new QGroupBox("Информация о системе");
    QFormLayout *infoLayout = new QFormLayout(infoGroup);

    QLabel *versionLabel = new QLabel("Версия 1.0");
    QLabel *userInfoLabel = new QLabel(
        QString("Пользователь: %1 (%2)")
            .arg(currentUser.value("full_name").toString())
            .arg(currentUser.value("role").toString())
        );

    infoLayout->addRow("Версия:", versionLabel);
    infoLayout->addRow("Статус:", userInfoLabel);

    layout->addWidget(appearanceGroup);
    layout->addWidget(infoGroup);
    layout->addStretch();

    tabs->addTab(settingsTab, "Настройки");

    // Подключаем сигнал изменения темы
    connect(themeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::changeTheme);
}

void MainWindow::changeTheme(int index) {
    QString theme = themeComboBox->itemData(index).toString();
    applyTheme(theme);

    // Сохраняем выбор темы
    QSettings settings;
    settings.setValue("theme", theme);
}

void MainWindow::applyTheme(const QString& theme) {
    if (theme == "Dark") {
        // Темная тема
        qApp->setStyleSheet(
            "QMainWindow, QDialog, QWidget {"
            "    background-color: #2b2b2b;"
            "    color: #ffffff;"
            "}"
            "QTabWidget::pane {"
            "    border: 1px solid #555;"
            "    background-color: #2b2b2b;"
            "}"
            "QTabBar::tab {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    padding: 8px 16px;"
            "    border: 1px solid #555;"
            "    border-bottom: none;"
            "    border-top-left-radius: 4px;"
            "    border-top-right-radius: 4px;"
            "}"
            "QTabBar::tab:selected {"
            "    background-color: #3498db;"
            "}"
            "QGroupBox {"
            "    color: #ffffff;"
            "    border: 1px solid #555;"
            "    border-radius: 5px;"
            "    margin-top: 10px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 10px;"
            "    padding: 0 5px;"
            "}"
            "QTableView {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    gridline-color: #555;"
            "    selection-background-color: #3498db;"
            "}"
            "QHeaderView::section {"
            "    background-color: #444;"
            "    color: #ffffff;"
            "    padding: 5px;"
            "    border: 1px solid #555;"
            "}"
            "QPushButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 5px;"
            "    padding: 8px 15px;"
            "}"
            "QPushButton:hover {"
            "    background-color: #2980b9;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #21618c;"
            "}"
            "QToolButton {"
            "    background-color: #3498db;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 5px;"
            "    padding: 8px 15px;"
            "}"
            "QToolButton:hover {"
            "    background-color: #2980b9;"
            "}"
            "QLineEdit {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    border: 1px solid #555;"
            "    border-radius: 3px;"
            "    padding: 5px;"
            "}"
            "QComboBox {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    border: 1px solid #555;"
            "    border-radius: 3px;"
            "    padding: 5px;"
            "}"
            "QComboBox QAbstractItemView {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    selection-background-color: #3498db;"
            "}"
            "QMenu {"
            "    background-color: #3b3b3b;"
            "    color: #ffffff;"
            "    border: 1px solid #555;"
            "}"
            "QMenu::item:selected {"
            "    background-color: #3498db;"
            "}"
            );
    } else {
        // Светлая тема (стиль по умолчанию)
        qApp->setStyleSheet(
            "QMainWindow, QDialog, QWidget {"
            "    background-color: #f0f0f0;"
            "    color: #000000;"
            "}"
            "QTabWidget::pane {"
            "    border: 1px solid #c0c0c0;"
            "    background-color: #f0f0f0;"
            "}"
            "QTabBar::tab {"
            "    background-color: #e0e0e0;"
            "    color: #000000;"
            "    padding: 8px 16px;"
            "    border: 1px solid #c0c0c0;"
            "    border-bottom: none;"
            "    border-top-left-radius: 4px;"
            "    border-top-right-radius: 4px;"
            "}"
            "QTabBar::tab:selected {"
            "    background-color: #3498db;"
            "    color: white;"
            "}"
            "QGroupBox {"
            "    color: #34495e;"
            "    border: 1px solid #c0c0c0;"
            "    border-radius: 5px;"
            "    margin-top: 10px;"
            "}"
            "QGroupBox::title {"
            "    subcontrol-origin: margin;"
            "    left: 10px;"
            "    padding: 0 5px;"
            "}"
            // Остальные стили для светлой темы можно оставить по умолчанию
            );
    }
}

void MainWindow::updateWelcomeLabel() {
    QString roleName;
    QString role = currentUser.value("role").toString();
    if (role == "admin") roleName = "Администратор";
    else if (role == "teacher") roleName = "Преподаватель";
    else if (role == "student") roleName = "Студент";
    else roleName = role;

    welcomeLabel->setText(QString("Добро пожаловать, <b>%1</b> (%2)")
                              .arg(currentUser.value("full_name").toString())
                              .arg(roleName));
    welcomeLabel->setStyleSheet("QLabel { font-size: 16px; padding: 10px; }");
}

void MainWindow::showProfileDialog() {
    ProfileDialog dialog(currentUser, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString newFullName = dialog.getFullName();
        QString newPassword = dialog.getPassword();
        QString confirmPassword = dialog.getConfirmPassword();

        if (newFullName.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Полное имя не может быть пустым");
            return;
        }

        if (!newPassword.isEmpty() && newPassword != confirmPassword) {
            QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
            return;
        }

        QSqlQuery query;

        if (newPassword.isEmpty()) {
            // Обновляем только полное имя
            query.prepare("UPDATE users SET full_name = ? WHERE id = ?");
            query.addBindValue(newFullName);
            query.addBindValue(currentUser.value("id").toInt());
        } else {
            // Обновляем и имя, и пароль
            query.prepare("UPDATE users SET full_name = ?, password = ? WHERE id = ?");
            query.addBindValue(newFullName);
            query.addBindValue(newPassword);
            query.addBindValue(currentUser.value("id").toInt());
        }

        if (query.exec()) {
            // Обновляем текущую запись пользователя
            currentUser.setValue("full_name", newFullName);

            // Обновляем приветствие
            updateWelcomeLabel();

            QMessageBox::information(this, "Успех", "Профиль успешно обновлен");
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить профиль: " + query.lastError().text());
        }
    }
}

void MainWindow::logout() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Выход",
                                                              "Вы уверены, что хотите выйти из аккаунта?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Закрываем главное окно
        this->close();
        // Завершаем приложение с кодом 0, чтобы main.cpp мог перезапустить
        QApplication::exit(0);
    }
}

void MainWindow::setupAdminTab() {
    QWidget *adminTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(adminTab);

    booksModel = new QSqlTableModel(this);
    booksModel->setTable("books");
    booksModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    booksModel->select();

    // Set Russian headers
    booksModel->setHeaderData(1, Qt::Horizontal, "Название");
    booksModel->setHeaderData(2, Qt::Horizontal, "Автор");
    booksModel->setHeaderData(3, Qt::Horizontal, "ISBN");
    booksModel->setHeaderData(4, Qt::Horizontal, "Год");
    booksModel->setHeaderData(5, Qt::Horizontal, "Жанр");
    booksModel->setHeaderData(6, Qt::Horizontal, "Доступна");

    adminBooksView = new QTableView;
    adminBooksView->setModel(booksModel);
    adminBooksView->setSelectionMode(QTableView::SingleSelection);
    adminBooksView->setSelectionBehavior(QTableView::SelectRows);
    adminBooksView->resizeColumnsToContents();

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("Добавить книгу");
    QPushButton *editButton = new QPushButton("Редактировать книгу");
    QPushButton *deleteButton = new QPushButton("Удалить книгу");
    QPushButton *refreshButton = new QPushButton("Обновить");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton);

    layout->addWidget(adminBooksView);
    layout->addLayout(buttonLayout);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addBook);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editBook);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteBook);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshModels);

    tabs->addTab(adminTab, "Управление книгами");
}

void MainWindow::setupStudentTab() {
    QWidget *studentTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(studentTab);

    availableBooksModel = new QSqlQueryModel(this);
    availableBooksModel->setQuery("SELECT id, title, author, year, genre FROM books WHERE available = 1");
    availableBooksModel->setHeaderData(1, Qt::Horizontal, "Название");
    availableBooksModel->setHeaderData(2, Qt::Horizontal, "Автор");
    availableBooksModel->setHeaderData(3, Qt::Horizontal, "Год");
    availableBooksModel->setHeaderData(4, Qt::Horizontal, "Жанр");

    studentBooksView = new QTableView;
    studentBooksView->setModel(availableBooksModel);
    studentBooksView->setSelectionMode(QTableView::SingleSelection);
    studentBooksView->setSelectionBehavior(QTableView::SelectRows);
    studentBooksView->hideColumn(0); // Hide ID column

    QPushButton *rentButton = new QPushButton("Взять выбранную книгу");
    QPushButton *refreshButton = new QPushButton("Обновить");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(rentButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton);

    layout->addWidget(studentBooksView);
    layout->addLayout(buttonLayout);

    connect(rentButton, &QPushButton::clicked, this, &MainWindow::rentBook);
    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshModels);

    tabs->addTab(studentTab, "Каталог библиотеки");
}

void MainWindow::setupStudentRentalsTab() {
    QWidget *studentRentalsTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(studentRentalsTab);

    // Создаем модель для отображения аренд текущего студента
    studentRentalsModel = new QSqlQueryModel(this);

    // Обновляем данные
    QString studentId = currentUser.value("id").toString();
    QString queryStr = QString(
                           "SELECT b.title as 'Название книги', b.author as 'Автор', "
                           "r.rental_date as 'Дата взятия', r.due_date as 'Срок возврата', "
                           "r.return_date as 'Дата возврата', "
                           "CASE WHEN r.return_date IS NULL THEN 'В аренде' ELSE 'Возвращена' END as 'Статус' "
                           "FROM rentals r "
                           "JOIN books b ON r.book_id = b.id "
                           "WHERE r.user_id = %1 "
                           "ORDER BY r.rental_date DESC"
                           ).arg(studentId);

    studentRentalsModel->setQuery(queryStr);

    studentRentalsView = new QTableView;
    studentRentalsView->setModel(studentRentalsModel);
    studentRentalsView->setSelectionMode(QTableView::SingleSelection);
    studentRentalsView->setSelectionBehavior(QTableView::SelectRows);
    studentRentalsView->resizeColumnsToContents();

    QPushButton *refreshButton = new QPushButton("Обновить");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton);

    layout->addWidget(studentRentalsView);
    layout->addLayout(buttonLayout);

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshModels);

    tabs->addTab(studentRentalsTab, "Мои книги");
}

void MainWindow::setupTeacherTab() {
    QWidget *teacherTab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(teacherTab);

    rentalsModel = new QSqlQueryModel(this);
    rentalsModel->setQuery(
        "SELECT r.id, u.full_name as student, b.title as book, r.rental_date, r.due_date, r.return_date "
        "FROM rentals r "
        "JOIN users u ON r.user_id = u.id "
        "JOIN books b ON r.book_id = b.id "
        "ORDER BY r.rental_date DESC"
        );

    // Set Russian headers
    rentalsModel->setHeaderData(1, Qt::Horizontal, "Студент");
    rentalsModel->setHeaderData(2, Qt::Horizontal, "Книга");
    rentalsModel->setHeaderData(3, Qt::Horizontal, "Дата выдачи");
    rentalsModel->setHeaderData(4, Qt::Horizontal, "Срок возврата");
    rentalsModel->setHeaderData(5, Qt::Horizontal, "Дата возврата");

    teacherRentalsView = new QTableView;
    teacherRentalsView->setModel(rentalsModel);
    teacherRentalsView->setSelectionMode(QTableView::SingleSelection);
    teacherRentalsView->setSelectionBehavior(QTableView::SelectRows);

    QPushButton *refreshButton = new QPushButton("Обновить");
    QPushButton *returnButton = new QPushButton("Отметить как возвращенную");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(returnButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(refreshButton);

    layout->addWidget(teacherRentalsView);
    layout->addLayout(buttonLayout);

    connect(refreshButton, &QPushButton::clicked, this, &MainWindow::refreshModels);
    connect(returnButton, &QPushButton::clicked, this, &MainWindow::returnBook);

    tabs->addTab(teacherTab, "Обзор аренд");
}

void MainWindow::addBook() {
    int row = booksModel->rowCount();
    booksModel->insertRow(row);

    QModelIndex index = booksModel->index(row, 1); // Title column
    adminBooksView->setCurrentIndex(index);
    adminBooksView->edit(index);
}

void MainWindow::editBook() {
    QModelIndexList selected = adminBooksView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Информация", "Пожалуйста, выберите книгу для редактирования");
        return;
    }

    int row = selected.first().row();
    adminBooksView->edit(selected.first());
}

void MainWindow::deleteBook() {
    QModelIndexList selected = adminBooksView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Информация", "Пожалуйста, выберите книгу для удаления");
        return;
    }

    int row = selected.first().row();
    QString bookTitle = booksModel->data(booksModel->index(row, 1)).toString();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение удаления",
                                                              QString("Вы уверены, что хотите удалить '%1'?").arg(bookTitle),
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        booksModel->removeRow(row);
        booksModel->submitAll();
        refreshModels();
    }
}

void MainWindow::rentBook() {
    QModelIndexList selected = studentBooksView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Информация", "Пожалуйста, выберите книгу для аренды");
        return;
    }

    int row = selected.first().row();
    int bookId = availableBooksModel->data(availableBooksModel->index(row, 0)).toInt();
    QString bookTitle = availableBooksModel->data(availableBooksModel->index(row, 1)).toString();

    // For students, they can only rent for themselves
    QString studentName = currentUser.value("full_name").toString();

    QSqlQuery query;

    // Используем MySQL-совместимый синтаксис для дат
    QString rentSql = "INSERT INTO rentals (user_id, book_id, student_name, rental_date, due_date) "
                      "VALUES (?, ?, ?, CURDATE(), DATE_ADD(CURDATE(), INTERVAL 14 DAY))";

    query.prepare(rentSql);
    query.addBindValue(currentUser.value("id").toInt());
    query.addBindValue(bookId);
    query.addBindValue(studentName);

    qDebug() << "SQL запрос аренды:" << rentSql;
    qDebug() << "Параметры:" << currentUser.value("id").toInt() << bookId << studentName;

    if (query.exec()) {
        // Mark book as unavailable
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE books SET available = 0 WHERE id = ?");
        updateQuery.addBindValue(bookId);
        updateQuery.exec();

        QMessageBox::information(this, "Успех",
                                 QString("Вы успешно взяли книгу '%1'. Срок возврата: %2")
                                     .arg(bookTitle)
                                     .arg(QDate::currentDate().addDays(14).toString("dd.MM.yyyy")));
        refreshModels();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось взять книгу: " + query.lastError().text());
        qDebug() << "Ошибка SQL:" << query.lastError().text();
    }
}

void MainWindow::returnBook() {
    QModelIndexList selected = teacherRentalsView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::information(this, "Информация", "Пожалуйста, выберите аренду для отметки о возврате");
        return;
    }

    int row = selected.first().row();
    int rentalId = rentalsModel->data(rentalsModel->index(row, 0)).toInt();
    int bookId = -1;

    // Get the book ID for this rental
    QSqlQuery query;
    query.prepare("SELECT book_id FROM rentals WHERE id = ?");
    query.addBindValue(rentalId);
    if (query.exec() && query.next()) {
        bookId = query.value(0).toInt();
    }

    if (bookId != -1) {
        QSqlQuery updateRental;
        // Используем MySQL-совместимый синтаксис для даты
        updateRental.prepare("UPDATE rentals SET return_date = CURDATE() WHERE id = ?");
        updateRental.addBindValue(rentalId);

        QSqlQuery updateBook;
        updateBook.prepare("UPDATE books SET available = 1 WHERE id = ?");
        updateBook.addBindValue(bookId);

        if (updateRental.exec() && updateBook.exec()) {
            QMessageBox::information(this, "Успех", "Книга отмечена как возвращенная");
            refreshModels();
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось обновить записи");
        }
    }
}

void MainWindow::refreshModels() {
    qDebug() << "Обновление моделей...";

    // Обновляем booksModel с проверкой
    if (booksModel) {
        if (!booksModel->select()) {
            qDebug() << "Ошибка обновления booksModel:" << booksModel->lastError().text();
        } else {
            qDebug() << "booksModel обновлена успешно";
        }
    }

    // Безопасное обновление availableBooksModel
    if (availableBooksModel) {
        QSqlQuery query;
        if (!query.exec("SELECT id, title, author, year, genre FROM books WHERE available = 1")) {
            qDebug() << "Ошибка выполнения запроса для availableBooksModel:" << query.lastError().text();
        } else {
            availableBooksModel->setQuery(query);
            qDebug() << "availableBooksModel обновлена успешно, строк:" << availableBooksModel->rowCount();
        }
    }

    // Безопасное обновление rentalsModel
    if (rentalsModel) {
        QSqlQuery query;
        if (!query.exec("SELECT r.id, u.full_name as student, b.title as book, r.rental_date, r.due_date, r.return_date "
                        "FROM rentals r "
                        "JOIN users u ON r.user_id = u.id "
                        "JOIN books b ON r.book_id = b.id "
                        "ORDER BY r.rental_date DESC")) {
            qDebug() << "Ошибка выполнения запроса для rentalsModel:" << query.lastError().text();
        } else {
            rentalsModel->setQuery(query);
            qDebug() << "rentalsModel обновлена успешно, строк:" << rentalsModel->rowCount();
        }
    }

    // Безопасное обновление studentRentalsModel
    if (studentRentalsModel) {
        QString studentId = currentUser.value("id").toString();
        QString queryStr = QString(
                               "SELECT b.title as 'Название книги', b.author as 'Автор', "
                               "r.rental_date as 'Дата взятия', r.due_date as 'Срок возврата', "
                               "r.return_date as 'Дата возврата', "
                               "CASE WHEN r.return_date IS NULL THEN 'В аренде' ELSE 'Возвращена' END as 'Статус' "
                               "FROM rentals r "
                               "JOIN books b ON r.book_id = b.id "
                               "WHERE r.user_id = %1 "
                               "ORDER BY r.rental_date DESC"
                               ).arg(studentId);

        QSqlQuery query;
        if (!query.exec(queryStr)) {
            qDebug() << "Ошибка выполнения запроса для studentRentalsModel:" << query.lastError().text();
        } else {
            studentRentalsModel->setQuery(query);
            qDebug() << "studentRentalsModel обновлена успешно, строк:" << studentRentalsModel->rowCount();
        }

        if (studentRentalsView) {
            studentRentalsView->resizeColumnsToContents();
        }
    }

    qDebug() << "Обновление моделей завершено";
}
