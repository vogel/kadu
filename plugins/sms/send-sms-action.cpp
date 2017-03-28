/*
 * %kadu copyright begin%
 * Copyright 2016 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "send-sms-action.h"

#include "sms-actions.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "buddies/buddy-set.h"

SendSmsAction::SendSmsAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"phone"});
    setName(QStringLiteral("sendSmsAction"));
    setShortcut("kadu_sendsms");
    setText(tr("Send SMS..."));
    setType(ActionDescription::TypeGlobal);
}

SendSmsAction::~SendSmsAction()
{
}

void SendSmsAction::setSmsActions(SmsActions *smsActions)
{
    m_smsActions = smsActions;
}

void SendSmsAction::actionTriggered(QAction *sender, bool)
{
    auto action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    m_smsActions->newSms(action->context()->buddies().toBuddy().mobile());
}

#include "moc_send-sms-action.cpp"
