/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include "buddies/buddy-manager.h"
#include "buddies/model/buddy-list-model.h"
#include "buddies/model/buddy-manager-adapter.h"
#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "icons/icons-manager.h"
#include "icons/kadu-icon.h"
#include "os/generic/window-geometry-manager.h"
#include "plugin/plugin-injected-factory.h"
#include "talkable/filter/mobile-talkable-filter.h"
#include "talkable/talkable-converter.h"
#include "widgets/select-talkable-combo-box.h"
#include "windows/message-dialog.h"
#include "windows/progress-window.h"
#include "debug.h"

#include "plugins/history/history-plugin-object.h"
#include "plugins/history/history.h"

#include "scripts/sms-script-manager.h"
#include "mobile-number-manager.h"
#include "sms-external-sender.h"
#include "sms-gateway-manager.h"
#include "sms-gateway.h"
#include "sms-internal-sender.h"

#include "sms-dialog.h"

SmsDialog::SmsDialog(History *history, MobileNumberManager *mobileNumberManager, SmsGatewayManager *smsGatewayManager, SmsScriptsManager *smsScriptsManager, QWidget* parent) :
		QWidget{parent, Qt::Window},
		m_history{history},
		m_mobileNumberManager{mobileNumberManager},
		m_smsGatewayManager{smsGatewayManager},
		m_smsScriptsManager{smsScriptsManager},
		MaxLength{0}
{
	setWindowTitle(tr("Send SMS"));
	setAttribute(Qt::WA_DeleteOnClose);
}

SmsDialog::~SmsDialog()
{
}

void SmsDialog::setBuddyManager(BuddyManager *buddyManager)
{
	m_buddyManager = buddyManager;
}

void SmsDialog::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void SmsDialog::setIconsManager(IconsManager *iconsManager)
{
	m_iconsManager = iconsManager;
}

void SmsDialog::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void SmsDialog::setTalkableConverter(TalkableConverter *talkableConverter)
{
	m_talkableConverter = talkableConverter;
}

void SmsDialog::init()
{
	createGui();
	validate();

	configurationUpdated();

	new WindowGeometryManager(new ConfigFileVariantWrapper(m_configuration, "Sms", "SmsDialogGeometry"), QRect(200, 200, 400, 250), this);

	RecipientEdit->setFocus();
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

	RecipientComboBox = m_pluginInjectedFactory->makeInjected<SelectTalkableComboBox>(this);
	RecipientComboBox->addBeforeAction(new QAction(tr(" - Select recipient - "), RecipientComboBox));

	auto buddyListModel = m_pluginInjectedFactory->makeInjected<BuddyListModel>(RecipientComboBox);
	m_pluginInjectedFactory->makeInjected<BuddyManagerAdapter>(buddyListModel);

	RecipientComboBox->setBaseModel(buddyListModel);
	RecipientComboBox->addFilter(new MobileTalkableFilter(RecipientComboBox));

	connect(RecipientComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(recipientBuddyChanged()));
	recipientLayout->addWidget(RecipientComboBox);

	formLayout->addRow(tr("Recipient") + ':', recipientWidget);

	ProviderComboBox = new QComboBox(this);
	ProviderComboBox->addItem(tr("Select automatically"), QString());

	for (auto gateway : m_smsGatewayManager->items())
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

	SignatureEdit = new QLineEdit(m_configuration->deprecatedApi()->readEntry("SMS", "SmsNick"), this);
	connect(SignatureEdit, SIGNAL(returnPressed()), this, SLOT(editReturnPressed()));

	formLayout->addRow(tr("Signature") + ':', SignatureEdit);

	SaveInHistoryCheckBox = new QCheckBox(tr("Save SMS in history"), this);
	SaveInHistoryCheckBox->setChecked(true);

	formLayout->addRow(0, SaveInHistoryCheckBox);

	QDialogButtonBox *buttons = new QDialogButtonBox(this);
	mainLayout->addSpacing(16);
	mainLayout->addWidget(buttons);

	SendButton = new QPushButton(this);
	SendButton->setIcon(m_iconsManager->iconByPath(KaduIcon("go-next")));
	SendButton->setText(tr("&Send"));
	SendButton->setDefault(true);
	SendButton->setMaximumWidth(200);
	connect(SendButton, SIGNAL(clicked()), this, SLOT(editReturnPressed()));

	QPushButton *closeButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"));
	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	QPushButton *clearButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogResetButton), tr("Clear"));
	connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clear()));

	buttons->addButton(SendButton, QDialogButtonBox::ApplyRole);
	buttons->addButton(closeButton, QDialogButtonBox::RejectRole);
	buttons->addButton(clearButton, QDialogButtonBox::DestructiveRole);

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
	ContentEdit->setFont(m_configuration->deprecatedApi()->readFontEntry("Look", "ChatFont"));
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
	RecipientEdit->setText(m_talkableConverter->toBuddy(RecipientComboBox->currentTalkable()).mobile());
}

void SmsDialog::recipientNumberChanged(const QString &number)
{
	QString gatewayId = m_mobileNumberManager->gatewayId(RecipientEdit->text());
	ProviderComboBox->setCurrentIndex(ProviderComboBox->findData(gatewayId));

	if (-1 == ProviderComboBox->currentIndex())
		ProviderComboBox->setCurrentIndex(0);

	if (number.isEmpty())
	{
		RecipientComboBox->setCurrentTalkable(Talkable());
		return;
	}

	foreach (const Buddy &buddy, m_buddyManager->items())
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
	const SmsGateway &gateway = m_smsGatewayManager->byId(id);

	MaxLength = gateway.maxLength();

	if (0 == MaxLength)
		MaxLengthSuffixText.clear();
	else
		MaxLengthSuffixText = QString(" / %1").arg(gateway.maxLength());

	updateCounter();
}

void SmsDialog::gatewayAssigned(const QString &number, const QString &gatewayId)
{
	m_mobileNumberManager->registerNumber(number, gatewayId);
}

void SmsDialog::sendSms()
{
	kdebugf();

	SmsSender *sender;

	if (m_configuration->deprecatedApi()->readBoolEntry("SMS", "BuiltInApp"))
	{
		int gatewayIndex = ProviderComboBox->currentIndex();
		QString gatewayId = ProviderComboBox->itemData(gatewayIndex, Qt::UserRole).toString();
		sender = m_pluginInjectedFactory->makeInjected<SmsInternalSender>(m_smsGatewayManager, m_smsScriptsManager, RecipientEdit->text(), m_smsGatewayManager->byId(gatewayId), this);
	}
	else
	{
		if (m_configuration->deprecatedApi()->readEntry("SMS", "SmsApp").isEmpty())
		{
			MessageDialog::show(m_iconsManager->iconByPath(KaduIcon("dialog-warning")), tr("Kadu"),
					tr("SMS application was not specified. Visit the configuration section"), QMessageBox::Ok, this);
			kdebugm(KDEBUG_WARNING, "SMS application NOT specified. Exit.\n");
			return;
		}
		sender = m_pluginInjectedFactory->makeInjected<SmsExternalSender>(RecipientEdit->text(), this);
	}

	connect(sender, SIGNAL(gatewayAssigned(QString, QString)), this, SLOT(gatewayAssigned(QString, QString)));
	sender->setSignature(SignatureEdit->text());

	auto window = m_pluginInjectedFactory->makeInjected<ProgressWindow>(tr("Sending SMS..."));
	window->setCancellable(true);
	window->show();

	connect(window, SIGNAL(canceled()), sender, SLOT(cancel()));
	connect(sender, SIGNAL(canceled()), window, SLOT(reject()));

	connect(sender, SIGNAL(progress(QString,QString)), window, SLOT(addProgressEntry(QString,QString)));
	connect(sender, SIGNAL(finished(bool,QString,QString)), window, SLOT(progressFinished(bool,QString,QString)));

	if (SaveInHistoryCheckBox->isChecked())
		connect(sender, SIGNAL(smsSent(QString,QString)), this, SLOT(saveSmsInHistory(QString,QString)));

	sender->sendMessage(ContentEdit->toPlainText());

	kdebugf2();
}

void SmsDialog::updateCounter()
{
	LengthLabel->setText(QString::number(ContentEdit->toPlainText().length()) + MaxLengthSuffixText);

	validate();
}

void SmsDialog::saveSmsInHistory(const QString &number, const QString &message)
{
	if (m_history->currentStorage())
		m_history->currentStorage()->appendSms(number, message);
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

#include "moc_sms-dialog.cpp"
