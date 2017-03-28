/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddies/buddy-set.h"

#include <QtCore/QPointer>
#include <QtWidgets/QDialog>
#include <injeqt/injeqt.h>

class QLabel;
class QLineEdit;

class AccountsComboBox;
class AccountTalkableFilter;
class BuddyListModel;
class ChatManager;
class ChatStorage;
class ChatWidgetManager;
class Chat;
class CheckableBuddiesProxyModel;
class Configuration;
class IconsManager;
class InjectedFactory;
class ModelChain;

class AddRoomChatWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddRoomChatWindow(QWidget *parent = nullptr);
    virtual ~AddRoomChatWindow();

public slots:
    void accept();
    void start();

private:
    QPointer<ChatManager> m_chatManager;
    QPointer<ChatStorage> m_chatStorage;
    QPointer<ChatWidgetManager> m_chatWidgetManager;
    QPointer<Configuration> m_configuration;
    QPointer<IconsManager> m_iconsManager;
    QPointer<InjectedFactory> m_injectedFactory;

    AccountsComboBox *AccountCombo;
    QLineEdit *DisplayNameEdit;
    QLineEdit *RoomEdit;
    QLineEdit *NickEdit;
    QLineEdit *PasswordEdit;
    QLabel *ErrorLabel;
    QPushButton *AddButton;
    QPushButton *StartButton;

    void createGui();
    void displayErrorMessage(const QString &message);

    Chat computeChat() const;

private slots:
    INJEQT_SET void setChatManager(ChatManager *chatManager);
    INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
    INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
    INJEQT_INIT void init();

    void validateData();
};
