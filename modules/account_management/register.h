#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>

#include "gadu.h"

class LayoutHelper;
class QCheckBox;
class QLineEdit;
class QResizeEvent;

/**
	Dialog umo¿liwiaj±cy rejestracjê nowego u¿ytkownika
	@ingroup account_management
	@{
**/
class Register : public QWidget 
{
	Q_OBJECT

	public:
		Register(QDialog* parent = 0, const char *name = "register_user");
		~Register();

	private:
		QLineEdit *pwd;
		QLineEdit *pwd2;
		QLineEdit *mailedit;
		UinType uin;
		QCheckBox *cb_updateconfig;
		LayoutHelper *layoutHelper;

		void ask();
		void createConfig();

	private slots:
		void doRegister();
		void keyPressEvent(QKeyEvent *);

	public slots:
		void registered(bool ok, UinType uin);

	protected:
		virtual void resizeEvent(QResizeEvent *);
};

/** @} */

#endif
