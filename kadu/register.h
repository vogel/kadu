#ifndef REGISTER_H
#define REGISTER_H

#include <qdialog.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsocketnotifier.h>

#include "gadu.h"

/**
	Dialog umo¿liwiaj±cy rejestracjê nowego u¿ytkownika
**/
class Register : public QDialog {
	Q_OBJECT

	public:
		Register(QDialog* parent = 0, const char *name = 0);

	private:
		QLineEdit *pwd, *pwd2, *mailedit;
		QLabel *status;
		UinType uin;
		QCheckBox *updateconfig;

		void ask();	

	private slots:
		void doRegister();

	public slots:
		void registered(bool ok, UinType uin);

};

class Unregister : public QDialog {
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = 0);

	private:
		QLineEdit *uin, *pwd;
		QLabel *status;
		QCheckBox *updateconfig;

		void deleteConfig();

	private slots:
		void doUnregister();
	
	public slots:
		void unregistered(bool ok);

};

#endif

