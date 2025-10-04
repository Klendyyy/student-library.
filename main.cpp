#include <QApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include "LoginDialog.h"
#include "MainWindow.h"

bool initDatabase() {
    // ВАШИ ДАННЫЕ ДЛЯ ПОДКЛЮЧЕНИЯ
    QString dbType = "QMYSQL";
    QString host = "";
    QString dbName = "";
    QString username = "";
    QString password = "";
    int port = 3306;

    QSqlDatabase db = QSqlDatabase::addDatabase(dbType);
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(username);
    db.setPassword(password);
    db.setPort(port);

    if (!db.open()) {
        QMessageBox::critical(nullptr, "Ошибка подключения",
                              "Не удалось подключиться к базе данных:\n" + db.lastError().text());
        return false;
    }

    qDebug() << "Успешное подключение к базе данных";
    return true;
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setStyle("Fusion");

    // Цикл для перезапуска приложения при выходе
    int returnCode = 0;
    bool restart = true;

    while (restart) {
        restart = false;

        if (!initDatabase()) {
            return -1;
        }

        LoginDialog login;
        if (login.exec() == QDialog::Accepted) {
            MainWindow w(login.getCurrentUser());
            w.show();
            returnCode = app.exec(); // Запускаем главный цикл приложения

            // Если приложение завершилось с кодом 0, перезапускаем
            if (returnCode == 0) {
                restart = true;
            }
        } else {
            break; // Пользователь отменил вход
        }
    }

    return returnCode;
}
