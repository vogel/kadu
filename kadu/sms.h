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
		QProcess *smsProcess;

	private slots:
		void updateRecipient(const QString &);
		void updateList(const QString& newnumber);
		void sendSms();
		void updateCounter();
		void smsSigHandler();
		void sendSmsInternal();
};

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

class HttpClient : public QSocket
{
	Q_OBJECT
	
	private:
		QString Host;
		QString Path;
		QByteArray Data;
		QByteArray PostData;
		int ContentLength;
		QString CookieName;
		QString CookieValue;
		
	private slots:
		void onConnected();
		void onReadyRead();
		
	public:
		HttpClient(QString host);
		void get(QString path);
		void post(QString path,const QByteArray& data);
		const QByteArray& data();
		
	signals:
		void finished();
};

class SmsSender : public QObject
{
	Q_OBJECT

	private:
		QString Number;
		QString Message;
		QString Token;
		HttpClient* Http;
		enum SmsThreadState
		{
			SMS_LOADING_PAGE,
			SMS_LOADING_PICTURE,
			SMS_LOADING_RESULTS
		};
		SmsThreadState State;

	private slots:
		void onFinished();
		void onCodeEntered(const QString& code);

	public:
		SmsSender(QObject* parent,const QString& number,const QString& message);
		void run();
};

#endif
