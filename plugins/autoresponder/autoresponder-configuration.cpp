/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "autoresponder-configuration.h"

AutoresponderConfiguration::AutoresponderConfiguration() :
		RespondConferences(false), RespondOnlyFirst(false), StatusAvailable(false), StatusBusy(false), StatusInvisible(false)
{
}

AutoresponderConfiguration::AutoresponderConfiguration(const AutoresponderConfiguration &copyMe)
{
	AutoRespondText = copyMe.AutoRespondText;
	RespondConferences = copyMe.RespondConferences;
	RespondOnlyFirst = copyMe.RespondOnlyFirst;
	StatusAvailable = copyMe.StatusAvailable;
	StatusBusy = copyMe.StatusBusy;
	StatusInvisible = copyMe.StatusInvisible;
}

AutoresponderConfiguration & AutoresponderConfiguration::operator = (const AutoresponderConfiguration &copyMe)
{
	AutoRespondText = copyMe.AutoRespondText;
	RespondConferences = copyMe.RespondConferences;
	RespondOnlyFirst = copyMe.RespondOnlyFirst;
	StatusAvailable = copyMe.StatusAvailable;
	StatusBusy = copyMe.StatusBusy;
	StatusInvisible = copyMe.StatusInvisible;

	return *this;
}

void AutoresponderConfiguration::setAutoRespondText(const QString &autoRespondText)
{
	AutoRespondText = autoRespondText;
}

QString AutoresponderConfiguration::autoRespondText() const
{
	return AutoRespondText;
}

void AutoresponderConfiguration::setRespondConferences(bool respondConferences)
{
	RespondConferences = respondConferences;
}

bool AutoresponderConfiguration::respondConferences() const
{
	return RespondConferences;
}

void AutoresponderConfiguration::setRespondOnlyFirst(bool respondOnlyFirst)
{
	RespondOnlyFirst = respondOnlyFirst;
}

bool AutoresponderConfiguration::respondOnlyFirst() const
{
	return RespondOnlyFirst;
}

void AutoresponderConfiguration::setStatusAvailable(bool statusAvailable)
{
	StatusAvailable = statusAvailable;
}

bool AutoresponderConfiguration::statusAvailable() const
{
	return StatusAvailable;
}

void AutoresponderConfiguration::setStatusBusy(bool statusBusy)
{
	StatusBusy = statusBusy;
}

bool AutoresponderConfiguration::statusBusy() const
{
	return StatusBusy;
}

void AutoresponderConfiguration::setStatusInvisible(bool statusInvisible)
{
	StatusInvisible = statusInvisible;
}

bool AutoresponderConfiguration::statusInvisible() const
{
	return StatusInvisible;
}
