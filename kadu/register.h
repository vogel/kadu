#ifndef REGISTER_H
#define REGISTER_H

#include <qdialog.h>
#include <qtimer.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsocketnotifier.h>

#include "libgadu.h"
#include "misc.h"

class token : public QObject {
	Q_OBJECT

	public:
		token();
		~token();
		void getToken();

	private:
		struct gg_http *h;
		QSocketNotifier *snr;
		QSocketNotifier *snw;

		void deleteSocketNotifiers();
		void createSocketNotifiers();

	private slots:
		void socketEvent();
		void dataReceived();
		void dataSent();

	signals:
		void gotToken(struct gg_http *h);
		void tokenError();
};

/**
	Dialog umo¿liwiaj±cy rejestracjê nowego u¿ytkownika
**/
class Register : public QDialog {
	Q_OBJECT

	public:
		Register(QDialog* parent = 0, const char *name = 0);

	private:
		struct gg_http *h;
		struct gg_pubdir *p;
		ImageWidget *tokenimage;
		QLineEdit *pwd, *pwd2, *mailedit, *tokenedit;
		QLabel *status;
		uin_t uin;
		QCheckBox *updateconfig;
		QSocketNotifier *snr;
		QSocketNotifier *snw;
		QString tokenid;
		token token_handle;

		void ask();	
		void deleteSocketNotifiers();
		void createSocketNotifiers();

	private slots:
		void doRegister();
		void socketEvent();
		void dataReceived();
		void dataSent();
		void gotTokenReceived(struct gg_http *h);
		void tokenErrorReceived();

	protected:
		void closeEvent(QCloseEvent *e);
};

class Unregister : public QDialog {
	Q_OBJECT

	public:
		Unregister(QDialog* parent = 0, const char *name = 0);

	private:
		struct gg_http *h;
		struct gg_pubdir *p;
		QLineEdit *uin, *pwd, *mail;
		QLabel *status;
		QCheckBox *updateconfig;
		QSocketNotifier *snr;
		QSocketNotifier *snw;

		void deleteSocketNotifiers();
		void createSocketNotifiers();
		void deleteConfig();

	private slots:
		void doUnregister();
		void socketEvent();
		void dataReceived();
		void dataSent();

	protected:
		void closeEvent(QCloseEvent *e);
};

#endif
