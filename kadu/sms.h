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
#include <qthread.h>
#include <qnetwork.h>
#include <qurloperator.h>
#include <qnetworkprotocol.h>
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

class SmsThread : public QObject,public QThread
{
	Q_OBJECT

	private:
		QUrlOperator* UrlOp;
		QByteArray Data;
		enum SmsThreadState
		{
			SMS_LOADING_PAGE,
			SMS_LOADING_PICTURE,
			SMS_LOADING_RESULTS
		};
		SmsThreadState State;

	private slots:
		void onFinished(QNetworkOperation* op);
		void onCodeEntered(const QString& code);
		void onData(const QByteArray& data,QNetworkOperation* op);

	protected:
		void run();

	public:
		SmsThread(QObject* parent,const QString& number,const QString& message);
};

#endif
