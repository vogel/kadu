/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "widgets/modal-configuration-widget.h"
#include "exports.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QLineEdit;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class ChatConfigurationWidget;
class ChatConfigurationWidgetFactory;
class ChatConfigurationWidgetFactoryRepository;
class ChatEditWidget;
class ChatGroupsConfigurationWidget;
class ChatManager;
class ChatTypeManager;
class CompositeConfigurationValueStateNotifier;
class Configuration;
class GroupList;
class InjectedFactory;
class SimpleConfigurationValueStateNotifier;

class KADUAPI ChatDataWindow : public QWidget
{
	Q_OBJECT

public:
	explicit ChatDataWindow(const Chat &chat);
	virtual ~ChatDataWindow();

	QList<ChatConfigurationWidget *> chatConfigurationWidgets() const;

	void show();

	Chat chat() const { return MyChat; }

	QWidget * generalTab() const { return GeneralTab; }

signals:
	void widgetAdded(ChatConfigurationWidget *widget);
	void widgetRemoved(ChatConfigurationWidget *widget);

	void destroyed(const Chat &chat);
	void save();

protected:
	virtual void keyPressEvent(QKeyEvent *event);

private:
	QPointer<ChatConfigurationWidgetFactoryRepository> m_chatConfigurationWidgetFactoryRepository;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatTypeManager> m_chatTypeManager;
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;

	QMap<ChatConfigurationWidgetFactory *, ChatConfigurationWidget *> ChatConfigurationWidgets;

	CompositeConfigurationValueStateNotifier *ValueStateNotifier;
	SimpleConfigurationValueStateNotifier *SimpleStateNotifier;

	Chat MyChat;

	QTabWidget *TabWidget;
	QWidget *GeneralTab;

	QLineEdit *DisplayEdit;

	ChatEditWidget *EditWidget;
	ChatGroupsConfigurationWidget *GroupsTab;

	QPushButton *OkButton;
	QPushButton *ApplyButton;

	void createGui();
	void createButtons(QVBoxLayout *layout);

	void applyChatConfigurationWidgets();

private slots:
	INJEQT_SET void setChatConfigurationWidgetFactoryRepository(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatTypeManager(ChatTypeManager *chatTypeManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_INIT void init();

	void factoryRegistered(ChatConfigurationWidgetFactory *factory);
	void factoryUnregistered(ChatConfigurationWidgetFactory *factory);

	void displayEditChanged();
	void stateChangedSlot(ConfigurationValueState state);

	void updateChat();
	void updateChatAndClose();
	void chatRemoved(const Chat &buddy);

};
