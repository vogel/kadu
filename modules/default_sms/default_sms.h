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
		SmsIdeaGateway(QObject* parent, const char *name=0);
		~SmsIdeaGateway();
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
		static SmsGateway* isValidIdea(const QString& number, QObject* parent);
		static SmsGateway* isValidPlus(const QString& number, QObject* parent);
		static SmsGateway* isValidEra(const QString& number, QObject* parent);
	private:
		QString actualEraGateway;			
	public slots:
		void onCreateConfigDialog();
		void onCloseConfigDialog();
		void onApplyConfigDialog();
		void onChangeEraGateway(int gateway);			
};

extern SmsGatewaySlots* sms_gateway_slots;

#endif
