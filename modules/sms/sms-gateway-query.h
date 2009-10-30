#ifndef SMS_GATEWAY_QUERY_H
#define SMS_GATEWAY_QUERY_H

#include <QtCore/QBuffer>
#include <QtNetwork/QHttp>

class SmsGatewayQuery : public QObject
{
	Q_OBJECT
	
	QHttp *Query;
	QBuffer *QueryBuffer;

	private slots:
		void queryFinished(bool error);

	public:
		SmsGatewayQuery() {};
		~SmsGatewayQuery();
	public slots:
		void process(const QString& number);

	signals:
		void finished(bool success, const QString &provider);
};

#endif // SMS_GATEWAY_QUERY_H
