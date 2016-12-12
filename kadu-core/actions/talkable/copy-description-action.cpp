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

#include "copy-description-action.h"

#include "actions/action-context.h"
#include "actions/action.h"
#include "contacts/contact-set.h"

#include <QtGui/QClipboard>
#include <QtWidgets/QApplication>

CopyDescriptionAction::CopyDescriptionAction(QObject *parent) :
		// using C++ initializers breaks Qt's lupdate
		ActionDescription(parent)
{
	setIcon(KaduIcon{"edit-copy"});
	setName(QStringLiteral("copyDescriptionAction"));
	setText(tr("Copy Description"));
	setType(ActionDescription::TypeUser);
}

CopyDescriptionAction::~CopyDescriptionAction()
{
}

void CopyDescriptionAction::actionTriggered(QAction *sender, bool)
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

	QApplication::clipboard()->setText(description, QClipboard::Selection);
	QApplication::clipboard()->setText(description, QClipboard::Clipboard);
}

void CopyDescriptionAction::updateActionState(Action* action)
{
	action->setEnabled(!action->context()->contacts().toContact().currentStatus().description().isEmpty());
}

#include "moc_copy-description-action.cpp"
