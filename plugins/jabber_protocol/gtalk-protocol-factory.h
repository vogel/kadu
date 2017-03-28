/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "protocols/protocol-factory.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class FacebookDepreceatedMessage;
class PluginInjectedFactory;
class JabberProtocolMenuManager;

class GTalkProtocolFactory : public ProtocolFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(GTalkProtocolFactory)

public:
    Q_INVOKABLE explicit GTalkProtocolFactory(QObject *parent = nullptr);
    virtual ~GTalkProtocolFactory();

    virtual Protocol *createProtocolHandler(Account account) override;
    virtual AccountAddWidget *newAddAccountWidget(bool showButtons, QWidget *parent) override;
    virtual AccountCreateWidget *newCreateAccountWidget(bool showButtons, QWidget *parent) override;
    virtual AccountEditWidget *newEditAccountWidget(Account, QWidget *) override;
    virtual QWidget *newContactPersonalInfoWidget(Contact contact, QWidget *parent = nullptr) override;
    virtual ProtocolMenuManager *protocolMenuManager() override;
    virtual QList<StatusType> supportedStatusTypes() override;
    virtual Status adaptStatus(Status) const override;
    virtual QString idLabel() override;
    virtual QValidator::State validateId(QString id) override;
    virtual bool canRegister() override;

    virtual QString name() override
    {
        return "gmail/google talk";
    }
    virtual QString displayName() override
    {
        return "Gmail/Google Talk";
    }

    virtual KaduIcon icon() override;

private:
    QPointer<FacebookDepreceatedMessage> m_facebookDepreceatedMessage;
    QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
    QPointer<JabberProtocolMenuManager> m_jabberProtocolMenuManager;

    QList<StatusType> m_supportedStatusTypes;

private slots:
    INJEQT_SET void setFacebookDepreceatedMessage(FacebookDepreceatedMessage *facebookDepreceatedMessage);
    INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
    INJEQT_SET void setJabberProtocolMenuManager(JabberProtocolMenuManager *jabberProtocolMenuManager);
};
