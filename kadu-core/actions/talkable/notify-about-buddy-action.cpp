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

#include "notify-about-buddy-action.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "buddies/buddy-set.h"

NotifyAboutBuddyAction::NotifyAboutBuddyAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setCheckable(true);
	setIcon(KaduIcon{"kadu_icons/notify-about-buddy"});
	setName(QStringLiteral("notifyAboutUserAction"));
	setText(tr("Notify About Buddy"));
	setType(ActionDescription::TypeUser);
}

NotifyAboutBuddyAction::~NotifyAboutBuddyAction()
{
}

void NotifyAboutBuddyAction::actionTriggered(QAction *sender, bool toggled)
{
	auto action = qobject_cast<Action *>(sender);
	if (!action)
		return;

	auto const &buddies = action->context()->buddies();

	for (auto const &buddy : buddies)
	{
		if (buddy.isNull() || buddy.isAnonymous())
			continue;

		if (toggled)
			buddy.removeProperty("notify:Notify");
		else
			buddy.addProperty("notify:Notify", false, CustomProperties::Storable);
	}

	for (auto action : actions())
		if (action->context()->contacts().toBuddySet() == buddies)
			action->setChecked(toggled);
}

void NotifyAboutBuddyAction::updateActionState(Action *action)
{
	action->setEnabled(!action->context()->buddies().isEmpty());

	auto buddySet = action->context()->contacts().toBuddySet();
	auto notifyAll = std::all_of(std::begin(buddySet), std::end(buddySet), [](auto const &buddy){
		if (!buddy.data())
			return true;
		return buddy.data()->customProperties()->property("notify:Notify", true).toBool();
	});

	action->setChecked(notifyAll);
}

#include "moc_notify-about-buddy-action.cpp"
