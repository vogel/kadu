#ifndef _DEFAULT_SMS_H
#define _DEFAULT_SMS_H

#include "gui/windows/main-configuration-window.h"

#include "../sms/gui/windows/sms-image-dialog.h"
#include "../sms/sms-gateway.h"
#include "../sms/sms-gateway-manager.h"

/** @defgroup default_sms Default SMS
 * @{
 */
class SmsOrangeGateway : public SmsGateway
{
	Q_OBJECT

	QString Token;

	private slots:
		void onCodeEntered(const QString& code);

	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString);

	public:
		SmsOrangeGateway();
		~SmsOrangeGateway();
		virtual QString name() { return "orange"; };
		virtual QString displayName() { return "Orange"; };
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
};

class SmsPlusGateway : public SmsGateway
{
	Q_OBJECT

	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString);

	public:
		SmsPlusGateway();
		~SmsPlusGateway();
		virtual QString name() { return "plus"; };
		virtual QString displayName() { return "Plus"; };
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
};

class SmsEraGateway : public SmsGateway
{
	Q_OBJECT
	
	void createDefaultConfiguration();
	
	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString link);

	public:
		SmsEraGateway();
		~SmsEraGateway();
		static QString errorNumber(int nr);
		virtual QString name() { return "era"; };
		virtual QString displayName() { return "Era"; };
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
};

// FIXME: split it
class DefaultSmsConfigurationUiHandler : public ConfigurationUiHandler
{
	Q_OBJECT

	QStringList era_types;
	QStringList era_values;
	QString actualEraGateway;

	ConfigComboBox *eraGateway;
	ConfigLineEdit *sponsoredUser;
	ConfigLineEdit *sponsoredPassword;
	ConfigLineEdit *multimediaUser;
	ConfigLineEdit *multimediaPassword;

private slots:
	void onChangeEraGateway();

public:
	DefaultSmsConfigurationUiHandler(QObject *parent = 0);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

extern DefaultSmsConfigurationUiHandler *defaultSmsConfigurationUiHandler;

/** @} */

#endif
