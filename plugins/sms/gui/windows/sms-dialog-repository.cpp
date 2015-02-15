/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "sms-dialog-repository.h"

#include "gui/windows/sms-dialog.h"

SmsDialogRepository::SmsDialogRepository(QObject *parent) :
		QObject{parent}
{
}

SmsDialogRepository::~SmsDialogRepository()
{
	auto size = m_dialogs.size();
	for (auto i = size - 1; i >= 0; i--)
		delete m_dialogs.at(i);
}

void SmsDialogRepository::addDialog(SmsDialog *dialog)
{
	if (!dialog)
		return;

	if (m_dialogs.contains(dialog))
		return;

	m_dialogs.append(dialog);
	connect(dialog, SIGNAL(destroyed(QObject*)), this, SLOT(dialogDestroyed(QObject*)));
}

void SmsDialogRepository::removeDialog(SmsDialog *dialog)
{
	if (!dialog)
		return;

	auto index = m_dialogs.indexOf(dialog);
	if (index < 0)
		return;

	m_dialogs.remove(index);
	disconnect(dialog, nullptr, this, nullptr);
}

void SmsDialogRepository::dialogDestroyed(QObject *object)
{
	removeDialog(static_cast<SmsDialog *>(object));
}

#include "moc_sms-dialog-repository.cpp"
