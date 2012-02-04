/*
 * %kadu copyright begin%
 * Copyright 2010, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtGui/QAction>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddies-model.h"
#include "configuration/configuration-file.h"
#include "gui/widgets/select-talkable-combo-box.h"
#include "gui/windows/message-dialog.h"
#include "icons/kadu-icon.h"
#include "misc/misc.h"
#include "plugins/plugins-manager.h"
#include "talkable/filter/mobile-talkable-filter.h"
#include "debug.h"

#include "gui/windows/sms-progress-window.h"
#include "mobile-number-manager.h"
#include "sms-external-sender.h"
#include "sms-gateway.h"
#include "sms-gateway-manager.h"
#include "sms-internal-sender.h"

#include "sms-dialog.h"

SmsDialog::SmsDialog(QWidget* parent) :
		QWidget(parent, Qt::Window), MaxLength(0)
{
	kdebugf();

	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
	validate();

	configurationUpdated();

	loadWindowGeometry(this, "Sms", "SmsDialogGeometry", 200, 200, 400, 250);

	RecipientEdit->setFocus();

	PluginsManager::instance()->usePlugin("sms");
	kdebugf2();
}

SmsDialog::~SmsDialog()
{
	saveWindowGeometry(this, "Sms", "SmsDialogGeometry");

	PluginsManager::instance()->releasePlugin("sms");
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

	connect(RecipientEdit, SIGNAL(textChanged(QString)), this, SLOT(recipientNumberChanged(QString)));
	connect(RecipientEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));
	connect(RecipientEdit, SIGNAL(textChanged(QString)), this, SLOT(validate()));

	recipientLayout->addWidget(RecipientEdit);

	RecipientComboBox = new SelectTalkableComboBox(this);
	RecipientComboBox->addBeforeAction(new QAction(tr(" - Select recipient - "), RecipientComboBox));
	RecipientComboBox->setBaseModel(new BuddiesModel(RecipientComboBox));
	RecipientComboBox->addFilter(new MobileTalkableFilter(RecipientComboBox));

	connect(RecipientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(recipientBuddyChanged()));
	recipientLayout->addWidget(RecipientComboBox);

	formLayout->addRow(tr("Recipient") + ':', recipientWidget);

	ProviderComboBox = new QComboBox(this);
	ProviderComboBox->addItem(tr("Select automatically"), QString());

	foreach (const SmsGateway &gateway, SmsGatewayManager::instance()->items())
		ProviderComboBox->addItem(gateway.name(), gateway.id());

	connect(ProviderComboBox, SIGNAL(activated(int)), this, SLOT(gatewayActivated(int)));

	formLayout->addRow(tr("GSM provider") + ':', ProviderComboBox);

	ContentEdit = new QTextEdit(this);
	ContentEdit->setAcceptRichText(false);
	ContentEdit->setLineWrapMode(QTextEdit::WidgetWidth);
	ContentEdit->setTabChangesFocus(true);
	connect(ContentEdit, SIGNAL(textChanged()), this, SLOT(updateCounter()));

	formLayout->addRow(tr("Content") + ':', ContentEdit);

	LengthLabel = new QLabel("0", this);
	formLayout->addRow(0, LengthLabel);

	SignatureEdit = new QLineEdit(config_file.readEntry("SMS", "SmsNick"), this);
	connect(SignatureEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Signature") + ':', SignatureEdit);

	SaveInHistoryCheckBox = new QCheckBox(tr("Save SMS in history"), this);
	SaveInHistoryCheckBox->setChecked(true);

	formLayout->addRow(0, SaveInHistoryCheckBox);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addWidget(buttons);

	SendButton = new QPushButton(this);
	SendButton->setIcon(KaduIcon("go-next").icon());
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
}

void SmsDialog::validate()
{
	if (RecipientEdit->text().isEmpty())
	{
		SendButton->setEnabled(false);
		return;
	}

	int currentLength = ContentEdit->toPlainText().length();
	if (currentLength == 0)
		SendButton->setEnabled(false);
	else if (MaxLength == 0)
		SendButton->setEnabled(true);
	else
		SendButton->setEnabled(currentLength <= MaxLength);
}

void SmsDialog::configurationUpdated()
{
	ContentEdit->setFont(config_file.readFontEntry("Look", "ChatFont"));
}

void SmsDialog::setRecipient(const QString &phone)
{
	kdebugf();

	RecipientEdit->setText(phone);
	if (!phone.isEmpty())
		ContentEdit->setFocus();

	kdebugf2();
}

void SmsDialog::recipientBuddyChanged()
{
	RecipientEdit->setText(RecipientComboBox->currentTalkable().toBuddy().mobile());
}

void SmsDialog::recipientNumberChanged(const QString &number)
{
	QString gatewayId = MobileNumberManager::instance()->gatewayId(RecipientEdit->text());
	ProviderComboBox->setCurrentIndex(ProviderComboBox->findData(gatewayId));

	if (-1 == ProviderComboBox->currentIndex())
		ProviderComboBox->setCurrentIndex(0);

	if (number.isEmpty())
	{
		RecipientComboBox->setCurrentTalkable(Talkable());
		return;
	}

	foreach (const Buddy &buddy, BuddyManager::instance()->items())
		if (buddy.mobile() == number)
		{
			RecipientComboBox->setCurrentTalkable(buddy);
			return;
		}
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

void SmsDialog::gatewayActivated(int index)
{
	QString id = ProviderComboBox->itemData(index).toString();
	const SmsGateway &gateway = SmsGatewayManager::instance()->byId(id);

	MaxLength = gateway.maxLength();

	if (0 == MaxLength)
		MaxLengthSuffixText.clear();
	else
		MaxLengthSuffixText = QString(" / %1").arg(gateway.maxLength());

	updateCounter();
}

void SmsDialog::gatewayAssigned(const QString &number, const QString &gatewayId)
{
	MobileNumberManager::instance()->registerNumber(number, gatewayId);
}

void SmsDialog::sendSms()
{
	kdebugf();

	SmsSender *sender;

	if (config_file.readBoolEntry("SMS", "BuiltInApp"))
	{
		int gatewayIndex = ProviderComboBox->currentIndex();
		QString gatewayId = ProviderComboBox->itemData(gatewayIndex, Qt::UserRole).toString();
		sender = new SmsInternalSender(RecipientEdit->text(), SmsGatewayManager::instance()->byId(gatewayId), this);
	}
	else
	{
		if (config_file.readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageDialog::show(KaduIcon("dialog-warning"), tr("Kadu"),
					tr("SMS application was not specified. Visit the configuration section"), QMessageBox::Ok, this);
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		sender = new SmsExternalSender(RecipientEdit->text(), this);
	}

	connect(sender, SIGNAL(gatewayAssigned(QString, QString)), this, SLOT(gatewayAssigned(QString, QString)));
	sender->setSignature(SignatureEdit->text());

	SmsProgressWindow *window = new SmsProgressWindow(sender);
	window->show();

	sender->sendMessage(ContentEdit->toPlainText());

	kdebugf2();
}

void SmsDialog::updateCounter()
{
	LengthLabel->setText(QString::number(ContentEdit->toPlainText().length()) + MaxLengthSuffixText);

	validate();
}

void SmsDialog::clear()
{
	RecipientEdit->clear();
	RecipientComboBox->clear();
	ProviderComboBox->clear();
	ContentEdit->clear();
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
