/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SMS_H
#define SMS_H

#include <qdialog.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qprocess.h>
#include <qsocket.h>
#include <qimage.h>

class SmsImageWidget : public QWidget
{
	Q_OBJECT

	private:
		QImage Image;

	protected:
		virtual void paintEvent(QPaintEvent* e);

	public:
		SmsImageWidget(QWidget* parent,const QByteArray& image);
};

class SmsImageDialog : public QDialog
{
	Q_OBJECT

	private:
		QLineEdit* code_edit;

	private slots:
		void onReturnPressed();

	public:
		SmsImageDialog(QDialog* parent,const QByteArray& image);

	signals:
		void codeEntered(const QString& code);
};

class HttpClient : public QObject
{
	Q_OBJECT
	
	private:
		QSocket Socket;
		QString Host;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int Status;
		int ContentLength;
		QMap<QString,QString> Cookies;
		
	private slots:
		void onConnected();
		void onReadyRead();
		
	public:
		HttpClient();
		void setHost(QString host);
		void get(QString path);
		void post(QString path,const QByteArray& data);
		void post(QString path,const QString& data);
		int status();
		const QByteArray& data();
		QString encode(const QString& text);
		
	signals:
		void finished();
		void error();
};

class SmsSender : public QObject
{
	Q_OBJECT

	private:
		QString Number;
		QString Message;
		QString Token;
		HttpClient Http;
		enum SmsSenderProvider
		{
			SMS_IDEA,
			SMS_ERA,
			SMS_PLUS
		};
		SmsSenderProvider Provider;
		enum SmsSenderState
		{
			SMS_LOADING_PAGE,
			SMS_LOADING_PICTURE,
			SMS_LOADING_RESULTS
		};
		SmsSenderState State;

	private slots:
		void onFinished();
		void onError();
		void onCodeEntered(const QString& code);

	public:
		SmsSender(QObject* parent=0);
		void send(const QString& number,const QString& message);
		
	signals:
		void finished(bool success);
};

class Sms : public QDialog {
	Q_OBJECT
	
	public:
		Sms(const QString& altnick, QDialog* parent=0);

	private:
		QMultiLineEdit *body;
		QLineEdit *recipient;
		QComboBox* list;
		QLabel *smslen;
		QPushButton *b_send;
		QPushButton* b_send_int;
		QProcess *smsProcess;
		SmsSender Sender;

	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		void sendSms();
		void updateCounter();
		void smsSigHandler();
		void sendSmsInternal();
		void onSmsSenderFinished(bool success);
};


#endif
