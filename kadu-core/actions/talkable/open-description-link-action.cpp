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

#include "open-description-link-action.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "contacts/contact-set.h"
#include "os/generic/url-opener.h"
#include "url-handlers/url-handler-manager.h"

OpenDescriptionLinkAction::OpenDescriptionLinkAction(QObject *parent)
        :   // using C++ initializers breaks Qt's lupdate
          ActionDescription(parent)
{
    setIcon(KaduIcon{"go-jump"});
    setName(QStringLiteral("openDescriptionLinkAction"));
    setText(tr("Open Description Link in Browser"));
    setType(ActionDescription::TypeUser);
}

OpenDescriptionLinkAction::~OpenDescriptionLinkAction()
{
}

void OpenDescriptionLinkAction::setUrlHandlerManager(UrlHandlerManager *urlHandlerManager)
{
    m_urlHandlerManager = urlHandlerManager;
}

void OpenDescriptionLinkAction::setUrlOpener(UrlOpener *urlOpener)
{
    m_urlOpener = urlOpener;
}

void OpenDescriptionLinkAction::actionTriggered(QAction *sender, bool)
{
    auto action = qobject_cast<Action *>(sender);
    if (!action)
        return;

    auto const &contact = action->context()->contacts().toContact();
    if (!contact)
        return;

    auto const &description = contact.currentStatus().description();
    if (description.isEmpty())
        return;

    auto url = m_urlHandlerManager->urlRegExp();
    int idx_start = url.indexIn(description);
    if (idx_start >= 0)
        m_urlOpener->openUrl(description.mid(idx_start, url.matchedLength()).toUtf8());
}

void OpenDescriptionLinkAction::updateActionState(Action *action)
{
    action->setEnabled(
        action->context()->contacts().toContact().currentStatus().description().indexOf(
            m_urlHandlerManager->urlRegExp()) >= 0);
}

#include "moc_open-description-link-action.cpp"
