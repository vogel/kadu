#ifndef SMS_H
#define SMS_H

#include <qwidget.h>
#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprocess.h>
#include <qsocket.h>
#include <qimage.h>
#include <qmap.h>
#include <qmessagebox.h>
#include <qlistbox.h>
#include <qptrstack.h>

#include "configuration_aware_object.h"
#include "debug.h"
#include "http_client.h"
#include "main_configuration_window.h"
#include "misc.h"
#include "userlist.h"

/**
 * @defgroup sms SMS
 * @{
 */
class SmsImageDialog : public QDialog
{
	Q_OBJECT

	private:
		QLineEdit* code_edit;

	private slots:
		void onReturnPressed();

	public:
		SmsImageDialog(QWidget* parent,const QByteArray& image);
		void reject();

	signals:
		void codeEntered(const QString& code);
};

class SmsGateway : public QObject
{
	Q_OBJECT

	protected:
		enum GatewayState
		{
			SMS_LOADING_PAGE,
			SMS_LOADING_PICTURE,
			SMS_LOADING_RESULTS
		};
		GatewayState State;
		QString Number;
		QString Signature;
		QString Message;
		HttpClient Http;

	private slots:
		void httpError();
	protected slots:
		virtual void httpFinished()=0;
		virtual void httpRedirected(QString)=0;

	public:
		SmsGateway(QObject* parent, const char *name=0);
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature)=0;

	signals:
		void finished(bool success);
};

class SmsSender : public QObject
{
	Q_OBJECT

	private:
		SmsGateway* Gateway;

	private slots:
		void onFinished(bool success);

	public:
		SmsSender(QObject* parent=0, const char *name=0);
		~SmsSender();
	public slots:
		void send(const QString& number,const QString& message, const QString& contact, const QString& signature);

	signals:
		void finished(bool success);
};

/**
	Dialog umo¿liwiaj±cy wysy³anie SMS'ów
**/
class Sms : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	protected:
		virtual void configurationUpdated();

	public:
		Sms(const QString& altnick, QWidget* parent=0, const char *name=0);
		~Sms();
		void setRecipient(const QString& phone);

	private:
		QMultiLineEdit *body;
		QLineEdit *recipient;
		QComboBox* list;
		QLabel *smslen;
		QLabel *l_contact;
		QLineEdit *e_contact;
		QLabel *l_signature;
		QLineEdit *e_signature;
		QPushButton *b_send;
		QCheckBox *c_saveInHistory;
		QProcess *smsProcess;
		SmsSender Sender;

	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		void editReturnPressed();
		void sendSms();
		void updateCounter();
		void smsSigHandler();
		void onSmsSenderFinished(bool success);
};

typedef SmsGateway* isValidFunc(const QString&, QObject*);

// TODO: split into SmsConfigurationUiHandler and SmsGatewayManager and some more...
class SmsConfigurationUiHandler : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	int menuid;
	QMap<QString,isValidFunc*> gateways;

	QCheckBox *useBuiltIn;
	QLineEdit *customApp;
	QCheckBox *useCustomString;
	QLineEdit *customString;
	QListBox *gatewayListBox;

protected:
	virtual void configurationUpdated();

public:
	SmsConfigurationUiHandler();
	virtual ~SmsConfigurationUiHandler();

	void registerGateway(QString, isValidFunc* f);
	void unregisterGateway(QString);
	SmsGateway* getGateway(const QString& number);
	void newSms(QString nick);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

public slots:
	void onSmsBuildInCheckToggle(bool);
	void onSendSms();
	void onUserClicked(int button, QListBoxItem* item, const QPoint& pos);
	void onUserDblClicked(UserListElement elem);
	void onPopupMenuCreate();
	void onSendSmsToUser();
	void onUpButton();
	void onDownButton();
	void sendSmsActionActivated(const UserGroup* users);

};

extern SmsConfigurationUiHandler *smsConfigurationUiHandler;

/** @} */

#endif
