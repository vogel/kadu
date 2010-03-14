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
#include "sms-external-sender.h"
#include "sms-internal-sender.h"

#include "../history/history.h"

#include "sms-dialog.h"

SmsDialog::SmsDialog(const QString& altnick, QWidget* parent) :
		QWidget(parent, Qt::Window)
{
	kdebugf();

	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();

	if (altnick.isEmpty())
		RecipientEdit->setFocus();
	else
	{
		RecipientEdit->setText(BuddyManager::instance()->byDisplay(altnick, ActionReturnNull).mobile());
// 		SmsGateway * gateway = MobileNumberManager::instance()->gateway(RecipientEdit->text());
// 		if (gateway)
// 			ProviderComboBox->setCurrentIndex(ProviderComboBox->findData(gateway->name()));
	}

	RecipientComboBox->setCurrentIndex(RecipientComboBox->findText(altnick));

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

	RecipientEdit = new QLineEdit(this);
	RecipientEdit->setMinimumWidth(140);

	connect(RecipientEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateList(const QString&)));
	connect(RecipientEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	recipientLayout->addWidget(RecipientEdit);

	QStringList strlist; // lista kontaktow z przypisanym numerem telefonu

	foreach(Buddy c, BuddyManager::instance()->items())
		if (!c.mobile().isEmpty())
		 	strlist.append(c.display());
	strlist.sort();
	strlist.prepend(QString::null);

	RecipientComboBox = new QComboBox(this);
	RecipientComboBox->addItems(strlist);

	connect(RecipientComboBox, SIGNAL(activated(const QString&)), this, SLOT(updateRecipient(const QString &)));
	recipientLayout->addWidget(RecipientComboBox);

	formLayout->addRow(tr("Recipient") + ":", recipientWidget);

	ProviderComboBox = new QComboBox(this);
	ProviderComboBox->addItem(tr("Automatically select provider"), "");
// 	foreach (SmsGateway* gateway, SmsGatewayManager::instance()->gateways())
// 		ProviderComboBox->addItem(gateway->displayName(), gateway->name());

	formLayout->addRow(tr("GSM provider") + ":", ProviderComboBox);

	ContentEdit = new QTextEdit(this);
	ContentEdit->setLineWrapMode(QTextEdit::WidgetWidth);
	ContentEdit->setTabChangesFocus(true);
	connect(ContentEdit, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	formLayout->addRow(tr("Content") + ":", ContentEdit);

	LengthLabel = new QLabel("0", this);
	formLayout->addRow(0, LengthLabel);

	ContactEdit = new QLineEdit(this);
	connect(ContactEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Contact") + ":", ContactEdit);

	SignatureEdit = new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	connect(SignatureEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Signature") + ":", SignatureEdit);

	SaveInHistoryCheckBox = new QCheckBox(tr("Save SMS in history"), this);
	SaveInHistoryCheckBox->setChecked(true);

	formLayout->addRow(0, SaveInHistoryCheckBox);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addWidget(buttons);

	SendButton = new QPushButton(this);
	SendButton->setIcon(IconsManager::instance()->iconByPath("16x16/go-next.png"));
	SendButton->setText(tr("&Send"));
	SendButton->setDefault(true);
	SendButton->setMaximumWidth(200);
	connect(SendButton, SIGNAL(clicked()), this, SLOT(editReturnPressed()));

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	QPushButton *clearButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogDiscardButton), tr("Clear"));
	connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clear()));

	buttons->addButton(SendButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(clearButton, QDialogButtonBox::ResetRole);
	buttons->addButton(closeButton, QDialogButtonBox::DestructiveRole);

	resize(400, 250);

	loadWindowGeometry(this, "Sms", "SmsDialogGeometry", 200, 200, 400, 250);
}

void SmsDialog::configurationUpdated()
{
	ContentEdit->setFont(config_file.readFontEntry("Look", "ChatFont"));
}

void SmsDialog::setRecipient(const QString &phone)
{
	kdebugf();

	RecipientEdit->setText(phone);
	ContentEdit->setFocus();

	kdebugf2();
}

void SmsDialog::updateRecipient(const QString &newtext)
{
	kdebugf();
//	kdebugmf(KDEBUG_FUNCTION_START | KDEBUG_INFO, "'%s' %d %d\n", qPrintable(newtext), newtext.isEmpty(), userlist->containsAltNick(newtext));
	if (newtext.isEmpty())
	{
		RecipientEdit->clear();
		kdebugf2();
		return;
	}
	Buddy c = BuddyManager::instance()->byDisplay(newtext, ActionReturnNull);
	if (!c.isNull())
		RecipientEdit->setText(c.mobile());
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
			RecipientComboBox->setCurrentIndex(RecipientComboBox->findText(c.display()));
			kdebugf2();
			return;
		}
	RecipientComboBox->setCurrentIndex(-1);
	kdebugf2();
}

void SmsDialog::editReturnPressed()
{
	kdebugf();

	if (ContentEdit->toPlainText().isEmpty())
		ContentEdit->setFocus();
	else
		sendSms();

	kdebugf2();
}

void SmsDialog::sendSms()
{
	kdebugf();

	SmsSender *sender;

	if (config_file.readBoolEntry("SMS", "BuiltInApp"))
		sender = new SmsInternalSender(RecipientEdit->text(), QString::null, this);
	else
	{
		if (config_file.readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageDialog::msg(tr("Sms application was not specified. Visit the configuration section"), false, "32x32/dialog-warning.png", this);
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		sender = new SmsExternalSender(RecipientEdit->text(), this);
	}

	sender->setContact(ContactEdit->text());
	sender->setSignature(SignatureEdit->text());

	SmsProgressWindow *window = new SmsProgressWindow(sender);
	window->show();

	sender->sendMessage(ContentEdit->toPlainText());

	kdebugf2();
}

void SmsDialog::updateCounter()
{
	LengthLabel->setText(QString::number(ContentEdit->toPlainText().length()));
}

void SmsDialog::onSmsSenderFinished(bool success)
{
	kdebugf();

	if (success)
	{
		if (SaveInHistoryCheckBox->isChecked())
			History::instance()->appendSms(RecipientEdit->text(), ContentEdit->toPlainText());

// 		MobileNumberManager::instance()->registerNumber(recipient->text(), Sender.currentGateway()->name());

		ContentEdit->clear();
		ContactEdit->clear();
		SignatureEdit->clear();
		RecipientEdit->clear();
	}

	kdebugf2();
}

void SmsDialog::clear()
{
	RecipientEdit->clear();
	RecipientComboBox->clear();
	ProviderComboBox->clear();
	ContentEdit->clear();
	ContactEdit->clear();
	SignatureEdit->clear();
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
