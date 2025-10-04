#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QComboBox;
class QLabel;
class QGroupBox;
QT_END_NAMESPACE

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private slots:
    void attemptRegister();
    void validatePasswords();

private:
    void setupUI();

    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLineEdit *fullNameEdit;
    QComboBox *roleCombo;
    QPushButton *registerButton;
    QLabel *passwordMatchLabel;
};

#endif
