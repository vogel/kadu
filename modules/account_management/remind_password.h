#ifndef REMIND_PASSWORD_H
#define REMIND_PASSWORD_H

#include <qhbox.h>

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

#endif
