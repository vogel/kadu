#ifndef SMS_GATEWAY_QUERY_H
#define SMS_GATEWAY_QUERY_H

#include <QtCore/QBuffer>

class SmsGatewayQuery : public QObject
{
	Q_OBJECT

public:
	explicit SmsGatewayQuery(QObject *parent = 0);
	virtual ~SmsGatewayQuery();

public slots:
	void process(const QString& number);
	void queryFinished(const QString &provider);

signals:
	void finished(const QString &provider);

};

#endif // SMS_GATEWAY_QUERY_H
