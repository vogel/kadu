#ifndef PASSWORD_H
#define PASSWORD_H

#include <qobject.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdialog.h>

class RemindPassword : public QObject {
	Q_OBJECT
	public:
		RemindPassword();
		~RemindPassword();

	private slots:
		void reminded(bool ok);

	public slots:
		void start();

};

class ChangePassword : public QDialog {
	Q_OBJECT
	public:
		ChangePassword(QDialog *parent = 0, const char *name = 0);

	private:
		QLineEdit *emailedit, *newpwd, *newpwd2;
		QLabel *status;

	private slots:
		void start();
		void passwordChanged(bool ok);

};

#endif
