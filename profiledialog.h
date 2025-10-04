#ifndef PROFILEDIALOG_H
#define PROFILEDIALOG_H

#include "MainWindow.h"
#include <QDialog>
#include <QSqlRecord>

class QLineEdit;
class QLabel;
class QDialogButtonBox;

class ProfileDialog : public QDialog {
    Q_OBJECT

public:
    ProfileDialog(const QSqlRecord &userRecord, QWidget *parent = nullptr);

    QString getFullName() const;
    QString getPassword() const;
    QString getConfirmPassword() const;

private slots:
    void validatePasswords();

private:
    void setupUI();

    QSqlRecord currentUser;
    QLineEdit *fullNameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *passwordMatchLabel;
    QDialogButtonBox *buttonBox;
};

#endif
