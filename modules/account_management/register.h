#ifndef REGISTER_H
#define REGISTER_H

#include <qhbox.h>
#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qevent.h>
#include "gadu.h"

class LayoutHelper;
class QResizeEvent;

/**
	Dialog umo¿liwiaj±cy rejestracjê nowego u¿ytkownika
	@ingroup account_management
	@{
**/
class Register : public QHBox {
	Q_OBJECT

	public:
		Register(QDialog* parent = 0, const char *name = 0);
		~Register();

	private:
		QLineEdit *pwd;
		QLineEdit *pwd2;
		QLineEdit *mailedit;
		QLabel *status;
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
