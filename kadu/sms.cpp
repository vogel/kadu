/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qlayout.h>
#include <qmessagebox.h>
#include <klocale.h>

//
#include "kadu.h"
#include "sms.h"
//

Sms::Sms(const QString& altnick, QDialog* parent) : QDialog (parent, "Sms")
{
	QGridLayout * grid = new QGridLayout(this, 3, 4, 10, 3);

	body = new QMultiLineEdit(this);
	grid->addMultiCellWidget(body, 1, 1, 0, 3);
	body->setWordWrap(QMultiLineEdit::WidgetWidth);
	body->setFont(QFont(config.chatFont, config.chatFontSize));
	QObject::connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	if(altnick!="")
		recipient->setText(userlist.byAltNick(altnick).mobile);
	QObject::connect(recipient,SIGNAL(textChanged(const QString&)),this,SLOT(updateList(const QString&)));
	grid->addWidget(recipient, 0, 1);

	list = new QComboBox(this);
	list->insertItem("");
	for(int i=0; i<userlist.count(); i++)
	{
		if (userlist[i].mobile.length())
			list->insertItem(userlist[i].altnick);
	};
	list->setCurrentText(altnick);
	QObject::connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 3);

	QLabel *recilabel = new QLabel(this);
	recilabel->setText(i18n("Recipient"));
	grid->addWidget(recilabel, 0, 0);

	b_send = new QPushButton(this);
	b_send->setText(i18n("Send"));
	grid->addWidget(b_send, 3, 3);
	QObject::connect(b_send, SIGNAL(clicked()), this, SLOT(sendSms()));

	smslen = new QLabel(this);
	smslen->setText("0");
	grid->addWidget(smslen, 3, 0);

	resize(300,200);
	setCaption(i18n("Send SMS"));
}

void Sms::updateRecipient(const QString &newtext)
{
	if(newtext=="")
	{
		recipient->setText("");
		return;
	};
	for(int i=0; i<userlist.count(); i++)
		if(userlist[i].altnick==newtext)
		{
			recipient->setText(userlist[i].mobile);
			break;
		};
}

void Sms::updateList(const QString &newnumber)
{
	for(int i=0; i<userlist.count(); i++)
		if(userlist[i].mobile==newnumber)
		{
			list->setCurrentText(userlist[i].altnick);
			return;
		};
	list->setCurrentText("");
};

int Sms::sendSms(void) {
	b_send->setEnabled(false);
	body->setEnabled(false);

	if (QString::compare(config.smsapp,"")) {
		smsProcess = new QProcess(this);
		if (config.smscustomconf) {
			QString jakisstring;
			jakisstring.sprintf(config.smsconf, config.smsapp, recipient->text().toInt(), (const char *)body->text().local8Bit());
			smsProcess->addArgument(jakisstring);
			}
		else {
			smsProcess->addArgument(config.smsapp);
			smsProcess->addArgument(recipient->text());
			smsProcess->addArgument(body->text());
			}
		}
	else {
		QMessageBox::warning(this, i18n("SMS error"), i18n("Sms application was not specified. Visit the configuration section") );
		fprintf(stderr,"KK SMS application NOT specified. Exit.\n");
		return -1;
		}

	if (!smsProcess->start())
		QMessageBox::critical(this, i18n("SMS error"), i18n("Could not spawn child process. Check if the program is functional") );
	QObject::connect(smsProcess, SIGNAL(processExited()), this, SLOT(smsSigHandler()));
}

void Sms::smsSigHandler() {
	if (smsProcess->normalExit())
		QMessageBox::information(this, i18n("SMS sent"), i18n("The process exited normally. The SMS should be on its way"));
	else
		QMessageBox::warning(this, i18n("SMS not sent"), i18n("The process exited abnormally. The SMS may not be sent"));

	b_send->setEnabled(true);
	body->setEnabled(true);
	body->clear();
}

void Sms::updateCounter() {
	char len[10];
	snprintf(len, sizeof(len), "%d", body->text().length());
	smslen->setText(len);
}

#include "sms.moc"
