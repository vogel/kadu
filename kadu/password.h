#ifndef PASSWORD_H
#define PASSWORD_H

#include <qhbox.h>
#include <qlineedit.h>

class RemindPassword : public QHBox {
	Q_OBJECT
	public:
		RemindPassword();
		~RemindPassword();

	private slots:
		void reminded(bool ok);

	public slots:
		void start();

};

class ChangePassword : public QHBox {
	Q_OBJECT
	public:
		ChangePassword(QDialog *parent = 0, const char *name = 0);
		~ChangePassword();

	private:
		QLineEdit *emailedit, *newpwd, *newpwd2;

	private slots:
		void start();
		void passwordChanged(bool ok);
		void keyPressEvent(QKeyEvent *);

};

#endif
