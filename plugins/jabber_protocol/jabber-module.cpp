/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "jabber-module.h"

#include "actions/ask-for-subscription-action.h"
#include "actions/jabber-actions.h"
#include "actions/jabber-protocol-menu-manager.h"
#include "actions/remove-subscription-action.h"
#include "actions/resend-subscription-action.h"
#include "actions/show-xml-console-action.h"
#include "facebook-depreceated-message.h"
#include "gtalk-protocol-factory.h"
#include "jabber-plugin-object.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-dom-visitor-provider.h"
#include "jabber-url-handler.h"

JabberModule::JabberModule()
{
    add_type<AskForSubscriptionAction>();
    add_type<FacebookDepreceatedMessage>();
    add_type<GTalkProtocolFactory>();
    add_type<JabberActions>();
    add_type<JabberPluginObject>();
    add_type<JabberProtocolFactory>();
    add_type<JabberProtocolMenuManager>();
    add_type<JabberUrlDomVisitorProvider>();
    add_type<JabberUrlHandler>();
    add_type<RemoveSubscriptionAction>();
    add_type<ResendSubscriptionAction>();
    add_type<ShowXmlConsoleAction>();
}
