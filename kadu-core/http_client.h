#ifndef KADU_HTTP_CLIENT_H
#define KADU_HTTP_CLIENT_H

#include <QByteArray>
#include <QTcpSocket>

// TODO: replace with QHttp
class HttpClient : public QObject
{
	Q_OBJECT

	QTcpSocket Socket;
	QString Host;
	QString Referer;
	QString Path;
	QByteArray Data;
	QByteArray PostData;

	int StatusCode;
	bool HeaderParsed;

	unsigned int ContentLength;
	bool ContentLengthNotFound;

	QMap<QString, QString> Cookies;

private slots:
	void onConnected();
	void onReadyRead();
	void onConnectionClosed();

public:
	HttpClient();
	int status() const;
	const QByteArray & data() const;
	static QString encode(const QString &text);

	const QString & cookie(const QString &name) const;
	const QMap<QString, QString> & cookies() const;
	void setCookie(const QString & name, const QString &value);

public slots:
	void setHost(const QString &host);
	void get(const QString &path);
	void post(const QString &path, const QByteArray &data);
	void post(const QString &path, const QString &data);

signals:
	void finished();
	void redirected(QString link);
	void error();

};

#endif
