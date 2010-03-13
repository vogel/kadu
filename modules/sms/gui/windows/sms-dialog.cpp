/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
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
#include "gui/windows/kadu-window.h"
#include "gui/windows/message-dialog.h"
#include "gui/hot-key.h"
#include "icons-manager.h"

#include "modules.h"
#include "misc/path-conversion.h"

#include "gui/windows/sms-progress-window.h"

#include "../history/history.h"

#include "sms-dialog.h"

SmsDialog::SmsDialog(const QString& altnick, QWidget* parent) : QWidget(parent, Qt::Window),
	body(0), recipient(0), list(0), smslen(0), e_contact(0),
	e_signature(0), b_send(0), c_saveInHistory(0), smsProcess(0)
{
	kdebugf();

	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();

	if (altnick.isEmpty())
		recipient->setFocus();
	else
	{
		recipient->setText(BuddyManager::instance()->byDisplay(altnick, ActionReturnNull).mobile());
		SmsGateway * gateway = MobileNumberManager::instance()->gateway(recipient->text());
		if (gateway)
			ProvidersList->setCurrentIndex(ProvidersList->findData(gateway->name()));
	}

	list->setCurrentIndex(list->findText(altnick));

	configurationUpdated();

	ModulesManager::instance()->moduleIncUsageCount("sms");
	kdebugf2();
}

SmsDialog::~SmsDialog()
{
	saveWindowGeometry(this, "Sms", "SmsDialogGeometry");

	ModulesManager::instance()->moduleDecUsageCount("sms");
}

void SmsDialog::createGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);

	QWidget *formWidget = new QWidget(this);
	mainLayout->addWidget(formWidget);

	QFormLayout *formLayout = new QFormLayout(formWidget);

	QWidget *recipientWidget = new QWidget(this);
	QHBoxLayout *recipientLayout = new QHBoxLayout(recipientWidget);
	recipientLayout->setContentsMargins(0, 0, 0, 0);

	recipient = new QLineEdit(this);
	recipient->setMinimumWidth(140);

	connect(recipient, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
	connect(recipient, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	recipientLayout->addWidget(recipient);

	QStringList strlist; // lista kontaktow z przypisanym numerem telefonu

	foreach(Buddy c, BuddyManager::instance()->items())
		if (!c.mobile().isEmpty())
		 	strlist.append(c.display());
	strlist.sort();
	strlist.prepend(QString::null);

	list = new QComboBox(this);
	list->addItems(strlist);

	connect(list, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	recipientLayout->addWidget(list);

	formLayout->addRow(tr("Recipient") + ":", recipientWidget);

	ProvidersList = new QComboBox(this);
	ProvidersList->addItem(tr("Automatically select provider"), "");
	foreach (SmsGateway* gateway, SmsGatewayManager::instance()->gateways())
		ProvidersList->addItem(gateway->displayName(), gateway->name());

	formLayout->addRow(tr("Select GSM provider") + ":", ProvidersList);

	body = new QTextEdit(this);
	body->setLineWrapMode(QTextEdit::WidgetWidth);
	body->setTabChangesFocus(true);
	connect(body, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	formLayout->addRow(tr("Content") + ":", body);

	smslen = new QLabel("0", this);
	formLayout->addRow(0, smslen);

	e_contact = new QLineEdit(this);
	connect(e_contact, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Contact") + ":", e_contact);

	e_signature = new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	connect(e_signature, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Signature") + ":", e_signature);

	c_saveInHistory = new QCheckBox(tr("Save SMS in history"), this);
	c_saveInHistory->setChecked(true);

	formLayout->addRow(0, c_saveInHistory);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addWidget(buttons);

	b_send = new QPushButton(this);
	b_send->setIcon(IconsManager::instance()->iconByPath("16x16/go-next.png"));
	b_send->setText(tr("&Send"));
	b_send->setDefault(true);
	b_send->setMaximumWidth(200);
	connect(b_send, SIGNAL(clicked()), this, SLOT(editReturnPressed()));

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	QPushButton *clearButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("Clear"));
	connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clear()));

	buttons->addButton(b_send, QDialogButtonBox::ApplyRole);
	buttons->addButton(clearButton, QDialogButtonBox::ResetRole);
	buttons->addButton(closeButton, QDialogButtonBox::DestructiveRole);

	resize(400, 250);

	loadWindowGeometry(this, "Sms", "SmsDialogGeometry", 200, 200, 400, 250);
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

	if (config_file.readBoolEntry("SMS", "BuiltInApp"))
	{
		SmsSender *sender = new SmsSender(recipient->text(), QString::null, this);
		sender->setContact(e_contact->text());
		sender->setSignature(e_signature->text());

		SmsProgressWindow *window = new SmsProgressWindow(sender);
		window->show();

		sender->sendMessage(body->toPlainText());
	}
	else
	{
		if (config_file.readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageDialog::msg(tr("Sms application was not specified. Visit the configuration section"), false, "32x32/dialog-warning.png", this);
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
			MessageDialog::msg(tr("Could not spawn child process. Check if the program is functional"), false, "32x32/dialog-warning.png", this);
		connect(smsProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(smsSigHandler()));
	}
	kdebugf2();
}

void SmsDialog::smsSigHandler()
{
	kdebugf();
	if (smsProcess->exitStatus() == QProcess::NormalExit)
		MessageDialog::msg(tr("The process exited normally. The SMS should be on its way"), false, "32x32/dialog-information.png", this);
	else
		MessageDialog::msg(tr("The process exited abnormally. The SMS may not be sent"), false, "32x32/dialog-warning.png", this);
	delete smsProcess;
	smsProcess = 0;

	c_saveInHistory->setEnabled(true);
	e_contact->setEnabled(true);
	e_signature->setEnabled(true);
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

// 		MobileNumberManager::instance()->registerNumber(recipient->text(), Sender.currentGateway()->name());

		body->clear();
		e_contact->clear();
		e_signature->clear();
		recipient->clear();
	}

	b_send->setEnabled(true);
	body->setEnabled(true);
	e_contact->setEnabled(true);
	e_signature->setEnabled(true);
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
