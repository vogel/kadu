#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qevent.h>

class RemindPassword : public QHBox
{
	Q_OBJECT

	private:
		QLineEdit* EmailEdit;

	private slots:

		void start();
		void reminded(bool ok);
		void keyPressEvent(QKeyEvent *);

	public:
		RemindPassword(QDialog *parent = 0, const char *name = 0);
		~RemindPassword();
};

#endif
