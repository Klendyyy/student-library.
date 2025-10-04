#include "RegisterDialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Регистрация нового аккаунта");
    setMinimumSize(500, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
}

void RegisterDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *registerGroup = new QGroupBox("Создание нового аккаунта");
    registerGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #34495e; }");

    QFormLayout *formLayout = new QFormLayout(registerGroup);

    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Выберите имя пользователя");
    usernameEdit->setMinimumHeight(35);
    usernameEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    fullNameEdit = new QLineEdit;
    fullNameEdit->setPlaceholderText("Введите ваше полное имя");
    fullNameEdit->setMinimumHeight(35);
    fullNameEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);
    passwordEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    confirmPasswordEdit = new QLineEdit;
    confirmPasswordEdit->setPlaceholderText("Подтвердите пароль");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setMinimumHeight(35);
    confirmPasswordEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    roleCombo = new QComboBox;
    roleCombo->addItems({"student", "teacher"});
    roleCombo->setMinimumHeight(35);
    roleCombo->setStyleSheet("QComboBox { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    passwordMatchLabel = new QLabel;
    passwordMatchLabel->setStyleSheet("QLabel { color: red; font-size: 12px; }");

    registerButton = new QPushButton("Зарегистрироваться");
    registerButton->setMinimumHeight(40);
    registerButton->setStyleSheet(
        "QPushButton { background-color: #2ecc71; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #27ae60; }"
        "QPushButton:disabled { background-color: #bdc3c7; }"
        );

    formLayout->addRow("Имя пользователя:", usernameEdit);
    formLayout->addRow("Полное имя:", fullNameEdit);
    formLayout->addRow("Пароль:", passwordEdit);
    formLayout->addRow("Подтверждение пароля:", confirmPasswordEdit);
    formLayout->addRow("", passwordMatchLabel);
    formLayout->addRow("Роль:", roleCombo);
    formLayout->addRow(registerButton);

    mainLayout->addWidget(registerGroup);

    // Connections
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::attemptRegister);
    connect(passwordEdit, &QLineEdit::textChanged, this, &RegisterDialog::validatePasswords);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterDialog::validatePasswords);
}

void RegisterDialog::validatePasswords() {
    QString password = passwordEdit->text();
    QString confirm = confirmPasswordEdit->text();

    if (confirm.isEmpty()) {
        passwordMatchLabel->clear();
        registerButton->setEnabled(true);
    } else if (password != confirm) {
        passwordMatchLabel->setText("Пароли не совпадают!");
        registerButton->setEnabled(false);
    } else {
        passwordMatchLabel->setText("Пароли совпадают!");
        passwordMatchLabel->setStyleSheet("QLabel { color: green; font-size: 12px; }");
        registerButton->setEnabled(true);
    }
}

void RegisterDialog::attemptRegister() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString fullName = fullNameEdit->text().trimmed();
    QString role = roleCombo->currentText();

    if (username.isEmpty() || password.isEmpty() || fullName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля");
        return;
    }

    if (password != confirmPasswordEdit->text()) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    // Проверка существующего имени пользователя - простой запрос без подготовки
    QString checkSql = QString("SELECT id FROM users WHERE username = '%1'").arg(username);
    QSqlQuery checkQuery;

    qDebug() << "Проверка пользователя:" << checkSql;

    if (!checkQuery.exec(checkSql)) {
        QMessageBox::critical(this, "Ошибка", "Ошибка проверки пользователя: " + checkQuery.lastError().text());
        return;
    }

    if (checkQuery.next()) {
        QMessageBox::warning(this, "Ошибка", "Имя пользователя уже существует");
        return;
    }

    // Вставка нового пользователя - простой запрос без подготовки
    QString insertSql = QString("INSERT INTO users (username, password, full_name, role) VALUES ('%1', '%2', '%3', '%4')")
                            .arg(username).arg(password).arg(fullName).arg(role);

    QSqlQuery insertQuery;
    qDebug() << "Вставка пользователя:" << insertSql;

    if (insertQuery.exec(insertSql)) {
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно!");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка", "Ошибка регистрации: " + insertQuery.lastError().text());
        qDebug() << "SQL Error:" << insertQuery.lastError().text();
    }
}
