#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>

#include "gadu.h"

class QCheckBox;
class QLineEdit;

/**
	Dialog umo¿liwiaj±cy rejestracjê nowego u¿ytkownika
	@ingroup account_management
	@{
**/
class Register : public QWidget 
{
	Q_OBJECT

		QLineEdit *pwd;
		QLineEdit *pwd2;
		QLineEdit *mailedit;
		UinType uin;
		QCheckBox *cb_updateconfig;

		void ask();
		void createConfig();

	private slots:
		void doRegister();
		void keyPressEvent(QKeyEvent *);

	public:
		Register(QDialog* parent = 0);
		~Register();

	public slots:
		void registered(bool ok, UinType uin);

};

/** @} */

#endif
