#ifndef SMS_GATEWAY_H
#define SMS_GATEWAY_H

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
		SmsGateway();
		virtual QString name() = 0;
		virtual QString displayName() = 0;
	public slots:
		virtual void send(const QString& number,const QString& message, const QString& contact, const QString& signature) = 0;

	signals:
		void finished(bool success);
};


#endif // SMS_GATEWAY_H
