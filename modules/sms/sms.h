#ifndef SMS_H
#define SMS_H

#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"

class QCheckBox;
class QComboBox;
class QListWidget;
class QProcess;
class QTextEdit;

/**
 * @defgroup sms SMS
 * @{
 */
class SMSAPI SmsImageDialog : public QDialog
{
	Q_OBJECT

		QLineEdit* code_edit;

	private slots:
		void onReturnPressed();

	public:
		SmsImageDialog(QWidget* parent, const QByteArray& image);
		void reject();

	signals:
		void codeEntered(const QString& code);
};

class SMSAPI SmsGateway : public QObject
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
		virtual void httpFinished() = 0;
		virtual void httpRedirected(QString) = 0;

	public:
		SmsGateway(QObject* parent);
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature) = 0;

	signals:
		void finished(bool success);
};

class SmsSender : public QObject
{
	Q_OBJECT

		SmsGateway* Gateway;

	private slots:
		void onFinished(bool success);

	public:
		SmsSender(QObject* parent = 0);
		~SmsSender();
	public slots:
		void send(const QString& number, const QString& message, const QString& contact, const QString& signature);

	signals:
		void finished(bool success);
};

/**
	Dialog umo�liwiaj�cy wysy�anie SMS'�w
**/
class SMSAPI Sms : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

		QTextEdit *body;
		QLineEdit *recipient;
		QComboBox *list;
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

	protected:
		virtual void configurationUpdated();
		virtual void keyPressEvent(QKeyEvent *e);

	public:
		Sms(const QString& altnick, QWidget* parent = 0);
		~Sms();
		void setRecipient(const QString& phone);
};

typedef SmsGateway* isValidFunc(const QString&, QObject*);

// TODO: split into SmsConfigurationUiHandler and SmsGatewayManager and some more...
class SMSAPI SmsConfigurationUiHandler : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	ActionDescription *sendSmsActionDescription;

	int menuid;
	QMap<QString,isValidFunc*> gateways;

	QCheckBox *useBuiltIn;
	QLineEdit *customApp;
	QCheckBox *useCustomString;
	QLineEdit *customString;
	QListWidget *gatewayListWidget;

	void createDefaultConfiguration();

private slots:
	void mainConfigurationWindowDestroyed();

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
	void onUserClicked(Contact contact);
	void onUserDblClicked(Chat *chat);
	void onUpButton();
	void onDownButton();
	void sendSmsActionActivated(QAction *sender, bool toggled);

};

extern SMSAPI SmsConfigurationUiHandler *smsConfigurationUiHandler;

/** @} */

#endif
