#ifndef _DEFAULT_SMS_H
#define _DEFAULT_SMS_H

#include "../sms/sms.h"

class SmsIdeaGateway : public SmsGateway
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
		SmsIdeaGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
		static bool isNumberCorrect(const QString& number);
};

class SmsPlusGateway : public SmsGateway
{
	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString);

	public:
		SmsPlusGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
		static bool isNumberCorrect(const QString& number);
};

class SmsEraGateway : public SmsGateway
{
	protected:
		virtual void httpFinished();
		virtual void httpRedirected(QString link);

	public:
		SmsEraGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature);
		static bool isNumberCorrect(const QString& number);
		static QString errorNumber(int nr);
};

class SmsGatewaySlots: public QObject
{

	Q_OBJECT
	
	public:
		SmsGatewaySlots();
		~SmsGatewaySlots();
		static SmsGateway* isValidIdea(QString& number, QObject* parent);
		static SmsGateway* isValidPlus(QString& number, QObject* parent);
		static SmsGateway* isValidEra(QString& number, QObject* parent);
	private:
		QString actualEraGateway;			
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void onChangeEraGateway(int gateway);			
};

extern SmsGatewaySlots* smsgatewayslots;

#endif
