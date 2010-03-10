#ifndef SMS_IMAGE_DIALOG_H
#define SMS_IMAGE_DIALOG_H

#include <QtGui/QDialog>
#include <QtScript/QScriptValue>

class QLabel;
class QLineEdit;
class QNetworkReply;

class SmsImageDialog : public QDialog
{
	Q_OBJECT

	QLineEdit *TokenEdit;
	QLabel *PixmapLabel;
	QNetworkReply *TokenNetworkReply;

	QScriptValue CallbackObject;
	QScriptValue CallbackMethod;

	void createGui();
	void loadTokenImage(const QString &tokenImageUrl);
	void result(const QString &value);

private slots:
	void tokenImageDownloaded();

public:
	SmsImageDialog(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod, QWidget *parent = 0);
	virtual ~SmsImageDialog();

public slots:
	virtual void accept();
	virtual void reject();

};

#endif //SMS_IMAGE_DIALOG_H
