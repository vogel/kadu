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
#include <qmap.h>

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
		void reject();

	signals:
		void codeEntered(const QString& code);
};

class HttpClient : public QObject
{
	Q_OBJECT
	
	private:
		QSocket Socket;
		QString Host;
		QString Referer;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int Status;
		bool HeaderParsed;
		int ContentLength;
		QMap<QString,QString> Cookies;
		
	private slots:
		void onConnected();
		void onReadyRead();
		void onConnectionClosed();
		
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

class SmsGateway : public QObject
{
	Q_OBJECT
	
	protected:
		enum GatewayState
		{
			SMS_LOADING_LOGIN_PAGE,
			SMS_LOADING_LOGIN_RESULTS,
			SMS_LOADING_PAGE,
			SMS_LOADING_PICTURE,
			SMS_LOADING_PREVIEW,
			SMS_LOADING_RESULTS
		};
		GatewayState State;
		QString Number;
		QString Message;
		HttpClient Http;

	private slots:
		void httpError();

	protected slots:
		virtual void httpFinished()=0;
		
	public:
		SmsGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message)=0;

	signals:
		void finished(bool success);
};

class SmsIdeaGateway : public SmsGateway
{
	Q_OBJECT
	
	private:
		QString Token;

	private slots:
		void onCodeEntered(const QString& code);

	protected:
		virtual void httpFinished();

	public:
		SmsIdeaGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message);
		static bool isNumberCorrect(const QString& number);
};

class SmsPlusGateway : public SmsGateway
{
	protected:
		virtual void httpFinished();

	public:
		SmsPlusGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message);
		static bool isNumberCorrect(const QString& number);
};

class SmsEraGateway : public SmsGateway
{
	protected:
		virtual void httpFinished();

	public:
		SmsEraGateway(QObject* parent);
		virtual void send(const QString& number,const QString& message);
		static bool isNumberCorrect(const QString& number);
};


class SmsSender : public QObject
{
	Q_OBJECT

	private slots:
		void onFinished(bool success);

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
		/**
			rejestruje opcje modulu Sms w oknie konfiguracji
		**/
		static void initModule();

	private:
		QMultiLineEdit *body;
		QLineEdit *recipient;
		QComboBox* list;
		QLabel *smslen;
		QPushButton *b_send;
		QProcess *smsProcess;
		SmsSender Sender;

	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		void sendSms();
		void updateCounter();
		void smsSigHandler();
		void onSmsSenderFinished(bool success);
};


#endif
