#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"

#include "sms_exports.h"
#include "sms-gateway.h"

class SmsSender : public QObject
{
	Q_OBJECT

		SmsGateway* CurrentGateway;
		QString Number;
		QString Message;
		QString Contact; 
		QString Signature;
		
		void gatewaySelected();

		void sendJavaScriptSms(const QString &gatewayId);

	private slots:
		void onFinished(bool success);

	public:
		SmsSender(QObject* parent = 0);
		~SmsSender();
		
		void findGatewayForNumber(const QString& number);
		
		SmsGateway * currentGateway() { return CurrentGateway; };

	public slots:
		void send(const QString& number, const QString& message, const QString& contact, const QString& signature, bool autoSelectProvider = true, QString provider = QString::null);
		void gatewayQueryDone(const QString &provider);

		void result();
		void failure(const QString &errorMessage);
	
	signals:
		void finished(bool success);
};

#endif // SMS_SENDER_H
