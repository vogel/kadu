/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QListWidget>
#include <QtCore/QProcess>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>

#include "configuration/configuration-file.h"
#include "buddies/buddy-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/buddies-list-widget.h"
#include "gui/widgets/buddies-list-view-menu-manager.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/image-widget.h"
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"

#include "../history/history.h"

#include "sms-dialog.h"

SmsDialog::SmsDialog(const QString& altnick, QWidget* parent) : QWidget(parent, Qt::Window),
	body(0), recipient(0), list(0), smslen(0), l_contact(0), e_contact(0), l_signature(0),
	e_signature(0), b_send(0), c_saveInHistory(0), smsProcess(0), Sender()
{
	kdebugf();

	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);

	QGridLayout *grid = new QGridLayout(this);
	grid->setColumnStretch(1, 5);
	
	QWidget *providerWidget = new QWidget(this);
	QGridLayout *widgetGrid = new QGridLayout(providerWidget);
	
	QLabel *providersListLabel = new QLabel(tr("Select GSM provider") + ":", this);
	widgetGrid->addWidget(providersListLabel, 0, 0, 1, 1);
	
	ProvidersList = new QComboBox(this);
	foreach (SmsGateway* gateway, SmsGatewayManager::instance()->gateways())
		ProvidersList->addItem(gateway->displayName(), gateway->name());
	widgetGrid->addWidget(ProvidersList, 0, 1, 1, 1);
	
	AutoSelectProvider = new QCheckBox(tr("Automatically select provider"), this);
	AutoSelectProvider->setChecked(false);
	widgetGrid->addWidget(AutoSelectProvider, 0, 2, 1, 1);
	
	connect(AutoSelectProvider, SIGNAL(toggled(bool)), ProvidersList, SLOT(setDisabled(bool)));
	connect(AutoSelectProvider, SIGNAL(toggled(bool)), providersListLabel, SLOT(setDisabled(bool)));
	
	grid->addWidget(providerWidget, 1, 0, 1, 3);

	body = new QTextEdit(this);
	grid->addWidget(body, 2, 0, 1, 3);
	body->setLineWrapMode(QTextEdit::WidgetWidth);
	body->setTabChangesFocus(true);
	connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);
	if (altnick.isEmpty())
		recipient->setFocus();
	else
	{
		recipient->setText(BuddyManager::instance()->byDisplay(altnick, ActionReturnNull).mobile());
		SmsGateway * gateway = MobileNumberManager::instance()->gateway(recipient->text());
		if (gateway)
			ProvidersList->setCurrentIndex(ProvidersList->findData(gateway->name()));
	}
	connect(recipient, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
	connect(recipient, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(recipient, 0, 1, 1, 1);

	QStringList strlist; // lista kontaktow z przypisanym numerem telefonu

	foreach(Buddy c, BuddyManager::instance()->items())
		if (!c.mobile().isEmpty())
		 	strlist.append(c.display());
	strlist.sort();
	strlist.prepend(QString::null);

	list = new QComboBox(this);
	list->addItems(strlist);
	list->setCurrentIndex(list->findText(altnick));
	connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	grid->addWidget(list, 0, 2, 1, 1);

	QLabel *recilabel = new QLabel(tr("Recipient"), this);
	grid->addWidget(recilabel, 0, 0, 1, 1);

	l_contact = new QLabel(tr("Contact"), this);
	grid->addWidget(l_contact, 3, 0, 1, 1);
	e_contact = new QLineEdit(this);
	connect(e_contact, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(e_contact, 3, 1, 1, 1);

	smslen = new QLabel("0", this);
	grid->addWidget(smslen, 3, 2, 1, 1, Qt::AlignRight);

	l_signature = new QLabel(tr("Signature"), this);
	grid->addWidget(l_signature, 4, 0, 1, 1);
	e_signature = new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	connect(e_signature, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	grid->addWidget(e_signature, 4, 1, 1, 1);

	c_saveInHistory = new QCheckBox(tr("Save SMS in history"), this);
	c_saveInHistory->setChecked(true);
	grid->addWidget(c_saveInHistory, 5, 0, 1, 2);

	b_send = new QPushButton(this);
	b_send->setIcon(IconsManager::instance()->iconByName("SendSMSButton"));
	b_send->setText(tr("&Send"));
	b_send->setDefault(true);
	b_send->setMaximumWidth(200);
	connect(b_send, SIGNAL(clicked()), this, SLOT(editReturnPressed()));
	grid->addWidget(b_send, 5, 2, 1, 1, Qt::AlignRight);

	resize(400, 250);

	connect(&Sender, SIGNAL(finished(bool)), this, SLOT(onSmsSenderFinished(bool)));

	configurationUpdated();
	
	loadWindowGeometry(this, "Sms", "SmsDialogGeometry", 200, 200, 400, 250);

	ModulesManager::instance()->moduleIncUsageCount("sms");
	kdebugf2();
}

SmsDialog::~SmsDialog()
{
	saveWindowGeometry(this, "Sms", "SmsDialogGeometry");

	ModulesManager::instance()->moduleDecUsageCount("sms");
}

void SmsDialog::configurationUpdated()
{
	body->setFont(config_file.readFontEntry("Look","ChatFont"));
}

void SmsDialog::setRecipient(const QString &phone)
{
    	kdebugf();
  	recipient->setText(phone);
  	body->setFocus();
  	kdebugf2();
}

void SmsDialog::updateRecipient(const QString &newtext)
{
	kdebugf();
//	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "'%s' %d %d\n", qPrintable(newtext), newtext.isEmpty(), userlist->containsAltNick(newtext));
	if (newtext.isEmpty())
	{
		recipient->clear();
		kdebugf2();
		return;
	}
	Buddy c = BuddyManager::instance()->byDisplay(newtext, ActionReturnNull);
	if (!c.isNull())
		recipient->setText(c.mobile());
	kdebugf2();
}

void SmsDialog::updateList(const QString &newnumber)
{
	kdebugf();
	if (newnumber.isEmpty())
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: new number is empty\n");
		return;
	}
	foreach(Buddy c, BuddyManager::instance()->items())
		if (c.mobile() == newnumber)
		{
			list->setCurrentIndex(list->findText(c.display()));
			kdebugf2();
			return;
		}
	list->setCurrentIndex(-1);
	kdebugf2();
}

void SmsDialog::editReturnPressed()
{
	kdebugf();

	if (body->toPlainText().isEmpty())
		body->setFocus();
	else
		sendSms();

	kdebugf2();
}

void SmsDialog::sendSms()
{
	kdebugf();
	b_send->setEnabled(false);
	body->setEnabled(false);
	e_contact->setEnabled(false);
	l_contact->setEnabled(false);
	e_signature->setEnabled(false);
	l_signature->setEnabled(false);
	c_saveInHistory->setEnabled(false);

	if (config_file.readBoolEntry("SMS", "BuiltInApp"))
	{
		Sender.send(recipient->text(), body->toPlainText(), e_contact->text(), e_signature->text(), 
			    AutoSelectProvider->isChecked(), ProvidersList->itemData(ProvidersList->currentIndex()).toString());
	}
	else
	{
		if (config_file.readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageDialog::msg(tr("Sms application was not specified. Visit the configuration section"), false, "Warning", this);
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		QString SmsAppPath = config_file.readEntry("SMS", "SmsApp");

		smsProcess = new QProcess(this);
		if (config_file.readBoolEntry("SMS", "UseCustomString")&&
			(!config_file.readBoolEntry("SMS", "BuiltInApp")))
		{
			QStringList args = config_file.readEntry("SMS", "SmsString").split(' ');

			args.replaceInStrings("%n", recipient->text());
			args.replaceInStrings("%n", body->toPlainText());
			
			smsProcess->start(SmsAppPath, args);
		}
		else
		{
			QStringList args(recipient->text());
			args.append(body->toPlainText());
			smsProcess->start(SmsAppPath, args);
		}

		if (!smsProcess->waitForStarted())
			MessageDialog::msg(tr("Could not spawn child process. Check if the program is functional"), false, "Warning", this);
		connect(smsProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(smsSigHandler()));
	}
	kdebugf2();
}

void SmsDialog::smsSigHandler()
{
	kdebugf();
	if (smsProcess->exitStatus() == QProcess::NormalExit)
		MessageDialog::msg(tr("The process exited normally. The SMS should be on its way"), false, "Information", this);
	else
		MessageDialog::msg(tr("The process exited abnormally. The SMS may not be sent"), false, "Warning", this);
	delete smsProcess;
	smsProcess = 0;

	c_saveInHistory->setEnabled(true);
	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
	b_send->setEnabled(true);
	body->setEnabled(true);
	body->clear();
	kdebugf2();
}

void SmsDialog::updateCounter()
{
	smslen->setText(QString::number(body->toPlainText().length()));
}

void SmsDialog::onSmsSenderFinished(bool success)
{
	kdebugf();

	if (success)
	{
		if (c_saveInHistory->isChecked())
			History::instance()->appendSms(recipient->text(), body->toPlainText());

		MobileNumberManager::instance()->registerNumber(recipient->text(), Sender.currentGateway()->name());

		body->clear();
		e_contact->clear();
		e_signature->clear();
		recipient->clear();
	}

	b_send->setEnabled(true);
	body->setEnabled(true);
	e_contact->setEnabled(true);
	l_contact->setEnabled(true);
	e_signature->setEnabled(true);
	l_signature->setEnabled(true);
	c_saveInHistory->setEnabled(true);
	kdebugf2();
}

void SmsDialog::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}
