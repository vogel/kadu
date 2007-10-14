#ifndef _DEFAULT_SMS_H
#define _DEFAULT_SMS_H

#include "main_configuration_window.h"

#include "../sms/sms.h"

/** @defgroup default_sms Default SMS
 * @{
 */
class SmsOrangeGateway : public SmsGateway
{
	Q_OBJECT

	private:
		QString Token;

	private slots:
		void onCodeEntered(const QString& code);

	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString);

	public:
		SmsOrangeGateway(QObject* parent, const char *name=0);
		~SmsOrangeGateway();
		static bool isNumberCorrect(const QString& number);
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
		SmsPlusGateway(QObject* parent, const char *name=0);
		~SmsPlusGateway();
		static bool isNumberCorrect(const QString& number);
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
};

class SmsEraGateway : public SmsGateway
{
	Q_OBJECT

		void import_0_5_0_configuration();
		void createDefaultConfiguration();

	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString link);

	public:
		SmsEraGateway(QObject* parent, const char *name=0);
		~SmsEraGateway();
		static bool isNumberCorrect(const QString& number);
		static QString errorNumber(int nr);
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
	static SmsGateway* isValidOrange(const QString& number, QObject* parent);
	static SmsGateway* isValidPlus(const QString& number, QObject* parent);
	static SmsGateway* isValidEra(const QString& number, QObject* parent);

	DefaultSmsConfigurationUiHandler(QObject *parent=0, const char *name=0);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

extern DefaultSmsConfigurationUiHandler *defaultSmsConfigurationUiHandler;

/** @} */

#endif
