#ifndef _DEFAULT_SMS_H
#define _DEFAULT_SMS_H

#include "../sms/sms.h"

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

class SmsGatewaySlots: public QObject
{
	Q_OBJECT
	
	public:
		SmsGatewaySlots(QObject *parent=0, const char *name=0);
		~SmsGatewaySlots();
		static SmsGateway* isValidOrange(const QString& number, QObject* parent);
		static SmsGateway* isValidPlus(const QString& number, QObject* parent);
		static SmsGateway* isValidEra(const QString& number, QObject* parent);
	private:
		QStringList era_types;
		QStringList era_values;
		QString actualEraGateway;
	public slots:
		void onCreateTabSMS();
		void onCloseTabSMS();
		void onApplyTabSMS();
		void onChangeEraGateway(int gateway);			
};

extern SmsGatewaySlots* sms_gateway_slots;

#endif
