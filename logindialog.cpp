#include "LoginDialog.h"
#include "RegisterDialog.h"
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QDebug>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Система управления библиотекой - Вход");
    setMinimumSize(500, 400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    setupConnections();
}

void LoginDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Title
    titleLabel = new QLabel("Система управления библиотекой");
    titleLabel->setStyleSheet("QLabel { color: #2c3e50; font-size: 24px; font-weight: bold; margin: 20px; }");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Login group box
    loginGroup = new QGroupBox("Вход в систему");
    loginGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #34495e; }");

    QFormLayout *formLayout = new QFormLayout(loginGroup);

    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Введите имя пользователя");
    usernameEdit->setMinimumHeight(35);
    usernameEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Введите пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);
    passwordEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    formLayout->addRow("Имя пользователя:", usernameEdit);
    formLayout->addRow("Пароль:", passwordEdit);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    loginButton = new QPushButton("Войти");
    registerButton = new QPushButton("Зарегистрироваться");

    loginButton->setMinimumHeight(40);
    registerButton->setMinimumHeight(40);

    loginButton->setStyleSheet(
        "QPushButton { background-color: #3498db; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #2980b9; }"
        "QPushButton:pressed { background-color: #21618c; }"
        );

    registerButton->setStyleSheet(
        "QPushButton { background-color: #2ecc71; color: white; border: none; border-radius: 5px; font-weight: bold; }"
        "QPushButton:hover { background-color: #27ae60; }"
        "QPushButton:pressed { background-color: #219653; }"
        );

    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(registerButton);

    formLayout->addRow(buttonLayout);

    mainLayout->addWidget(loginGroup);
    mainLayout->addStretch();
}

void LoginDialog::setupConnections() {
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::showRegisterDialog);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
}

void LoginDialog::attemptLogin() {
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, введите имя пользователя и пароль");
        return;
    }

    // Простой запрос без подготовки для избежания ошибок с параметрами
    QString sql = QString("SELECT * FROM users WHERE username = '%1' AND password = '%2'")
                      .arg(username).arg(password);

    QSqlQuery query;
    qDebug() << "Выполняем запрос:" << sql;

    if (query.exec(sql) && query.next()) {
        currentUser = query.record();
        qDebug() << "Успешный вход. Пользователь:" << currentUser.value("username").toString();
        accept();
    } else {
        qDebug() << "Ошибка входа:" << query.lastError().text();
        QMessageBox::warning(this, "Ошибка", "Неверное имя пользователя или пароль");
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::showRegisterDialog() {
    RegisterDialog registerDialog(this);
    if (registerDialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Успех", "Регистрация прошла успешно! Теперь вы можете войти в систему.");
        usernameEdit->clear();
        passwordEdit->clear();
        usernameEdit->setFocus();
    }
}
