#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QGroupBox;
QT_END_NAMESPACE

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QSqlRecord getCurrentUser() const { return currentUser; }
    bool isLoggedOut() const { return loggedOut; }

private slots:
    void attemptLogin();
    void showRegisterDialog();

private:
    void setupUI();
    void setupConnections();

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *titleLabel;
    QGroupBox *loginGroup;
    QSqlRecord currentUser;
    bool loggedOut = false;
};

#endif
