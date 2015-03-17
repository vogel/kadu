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

#ifndef CHAT_DATA_WINDOW_H
#define CHAT_DATA_WINDOW_H

#include <QtCore/QMap>
#include <QtWidgets/QWidget>

#include "chat/chat.h"
#include "gui/widgets/modal-configuration-widget.h"

#include "exports.h"

class QLineEdit;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class ChatConfigurationWidget;
class ChatConfigurationWidgetFactory;
class ChatConfigurationWidgetFactoryRepository;
class ChatEditWidget;
class ChatGroupsConfigurationWidget;
class CompositeConfigurationValueStateNotifier;
class GroupList;
class SimpleConfigurationValueStateNotifier;

class KADUAPI ChatDataWindow : public QWidget
{
	Q_OBJECT

	ChatConfigurationWidgetFactoryRepository *MyChatConfigurationWidgetFactoryRepository;
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
	void factoryRegistered(ChatConfigurationWidgetFactory *factory);
	void factoryUnregistered(ChatConfigurationWidgetFactory *factory);

	void displayEditChanged();
	void stateChangedSlot(ConfigurationValueState state);

	void updateChat();
	void updateChatAndClose();
	void chatRemoved(const Chat &buddy);

protected:
	virtual void keyPressEvent(QKeyEvent *event);

public:
	explicit ChatDataWindow(ChatConfigurationWidgetFactoryRepository *chatConfigurationWidgetFactoryRepository, const Chat &chat);
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

};

#endif // CHAT_DATA_WINDOW_H
