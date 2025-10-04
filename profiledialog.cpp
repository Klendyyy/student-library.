#include "ProfileDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>  // Добавлено включение для QPushButton

ProfileDialog::ProfileDialog(const QSqlRecord &userRecord, QWidget *parent)
    : QDialog(parent), currentUser(userRecord) {
    setWindowTitle("Редактирование профиля");
    setMinimumSize(400, 300);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
}

QString ProfileDialog::getFullName() const {
    return fullNameEdit->text().trimmed();
}

QString ProfileDialog::getPassword() const {
    return passwordEdit->text();
}

QString ProfileDialog::getConfirmPassword() const {
    return confirmPasswordEdit->text();
}

void ProfileDialog::validatePasswords() {
    QString password = passwordEdit->text();
    QString confirm = confirmPasswordEdit->text();

    if (confirm.isEmpty()) {
        passwordMatchLabel->clear();
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    } else if (password != confirm) {
        passwordMatchLabel->setText("Пароли не совпадают!");
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        passwordMatchLabel->setText("Пароли совпадают!");
        passwordMatchLabel->setStyleSheet("QLabel { color: green; font-size: 12px; }");
        buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void ProfileDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QGroupBox *profileGroup = new QGroupBox("Личные данные");
    profileGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #34495e; }");

    QFormLayout *formLayout = new QFormLayout(profileGroup);

    QLineEdit *usernameEdit = new QLineEdit;
    usernameEdit->setText(currentUser.value("username").toString());
    usernameEdit->setReadOnly(true);
    usernameEdit->setStyleSheet("QLineEdit { background-color: #f8f9fa; }");

    fullNameEdit = new QLineEdit;
    fullNameEdit->setText(currentUser.value("full_name").toString());
    fullNameEdit->setPlaceholderText("Введите ваше полное имя");
    fullNameEdit->setMinimumHeight(35);
    fullNameEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    QLabel *roleLabel = new QLabel;
    QString role = currentUser.value("role").toString();
    if (role == "admin") roleLabel->setText("Администратор");
    else if (role == "teacher") roleLabel->setText("Преподаватель");
    else if (role == "student") roleLabel->setText("Студент");
    else roleLabel->setText(role);

    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Введите новый пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);
    passwordEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    confirmPasswordEdit = new QLineEdit;
    confirmPasswordEdit->setPlaceholderText("Подтвердите новый пароль");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setMinimumHeight(35);
    confirmPasswordEdit->setStyleSheet("QLineEdit { padding: 5px; border: 1px solid #bdc3c7; border-radius: 3px; }");

    passwordMatchLabel = new QLabel;
    passwordMatchLabel->setStyleSheet("QLabel { color: red; font-size: 12px; }");

    formLayout->addRow("Имя пользователя:", usernameEdit);
    formLayout->addRow("Полное имя:", fullNameEdit);
    formLayout->addRow("Роль:", roleLabel);
    formLayout->addRow("Новый пароль:", passwordEdit);
    formLayout->addRow("Подтверждение пароля:", confirmPasswordEdit);
    formLayout->addRow("", passwordMatchLabel);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttonBox->button(QDialogButtonBox::Ok)->setText("Сохранить");
    buttonBox->button(QDialogButtonBox::Cancel)->setText("Отмена");

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(profileGroup);
    mainLayout->addWidget(buttonBox);

    connect(passwordEdit, &QLineEdit::textChanged, this, &ProfileDialog::validatePasswords);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &ProfileDialog::validatePasswords);
}
