#ifndef SMS_SENDER_H
#define SMS_SENDER_H

#include <QtCore/QBuffer>
#include <QtCore/QMap>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include <QtNetwork/QHttp>
#include <QtScript/QScriptValue>

#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "http_client.h"
#include "gui/actions/action.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/token-acceptor.h"

#include "sms_exports.h"
#include "sms-gateway.h"

class QNetworkReply;

class TokenReader;

class SmsSender : public QObject, public TokenAcceptor
{
	Q_OBJECT

	QString GatewayId;
	QString Number;
	QString Message;
	QString Contact;
	QString Signature;

	TokenReader *MyTokenReader;
	QNetworkReply *TokenReply;

	QScriptValue TokenCallbackObject;
	QScriptValue TokenCallbackMethod;

	void fixNumber();
	bool validateNumber();
	bool validateSignature();

	void queryForGateway();
	void gatewaySelected();

	void sendSms();

private slots:
    void tokenImageDownloaded();

public:
	explicit SmsSender(const QString &number, const QString &gatewayId = QString::null, QObject *parent = 0);
	virtual ~SmsSender();

	void setContact(const QString& contact);
	void setSignature(const QString& signature);
	void sendMessage(const QString& message);

	void setTokenReader(TokenReader *tokenReader);

	void findGatewayForNumber(const QString &number);

	virtual void tokenRead(const QString &tokenValue);

public slots:
	void gatewayQueryDone(const QString &gatewayId);
	void readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod);

	void result();
	void failure(const QString &errorMessage);

signals:
	void finished(bool success);

};

#endif // SMS_SENDER_H
