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

	QString GatewayId;
	QString Number;
	QString Message;
	QString Contact;
	QString Signature;

	void fixNumber();
	bool validateNumber();
	bool validateSignature();

	void queryForGateway();
	void gatewaySelected();

	void sendSms();

public:
	explicit SmsSender(const QString &number, const QString &gatewayId = QString::null, QObject *parent = 0);
	virtual ~SmsSender();

	void setContact(const QString& contact);
	void setSignature(const QString& signature);
	void sendMessage(const QString& message);

	void findGatewayForNumber(const QString &number);

public slots:
	void gatewayQueryDone(const QString &gatewayId);

	void result();
	void failure(const QString &errorMessage);

signals:
	void finished(bool success);

};

#endif // SMS_SENDER_H
