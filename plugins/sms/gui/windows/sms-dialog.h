/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

#include "buddies/buddy.h"
#include "configuration/configuration-aware-object.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QTextEdit;

class BuddyManager;
class Configuration;
class History;
class IconsManager;
class PluginInjectedFactory;
class MobileNumberManager;
class SelectTalkableComboBox;
class SmsGatewayManager;
class SmsScriptsManager;
class TalkableConverter;

class SmsDialog : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	QPointer<BuddyManager> m_buddyManager;
	QPointer<Configuration> m_configuration;
	QPointer<History> m_history;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<MobileNumberManager> m_mobileNumberManager;
	QPointer<SmsGatewayManager> m_smsGatewayManager;
	QPointer<SmsScriptsManager> m_smsScriptsManager;
	QPointer<TalkableConverter> m_talkableConverter;

	QLineEdit *RecipientEdit;
	SelectTalkableComboBox *RecipientComboBox;
	QComboBox *ProviderComboBox;
	QTextEdit *ContentEdit;
	QLabel *LengthLabel;
	QLineEdit *SignatureEdit;
	QPushButton *SendButton;
	QCheckBox *SaveInHistoryCheckBox;

	QString MaxLengthSuffixText;
	quint16 MaxLength;

	void createGui();

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setTalkableConverter(TalkableConverter *talkableConverter);
	INJEQT_INIT void init();

	void validate();

	void recipientBuddyChanged();
	void recipientNumberChanged(const QString &number);

	void editReturnPressed();
	void updateCounter();

	void gatewayActivated(int index);
	void gatewayAssigned(const QString &number, const QString &gatewayId);
	void sendSms();

	void saveSmsInHistory(const QString &number, const QString &message);

	void clear();

protected:
	virtual void configurationUpdated();
	virtual void keyPressEvent(QKeyEvent *e);

public:
	explicit SmsDialog(History *history, MobileNumberManager *mobileNumberManager, SmsGatewayManager *smsGatewayManager, SmsScriptsManager *smsScriptsManager, QWidget *parent = nullptr);
	virtual ~SmsDialog();

	void setRecipient(const QString &phone);

};
