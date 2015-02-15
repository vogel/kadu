/*
 * %kadu copyright begin%
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

#include "emoticon.h"

Emoticon::Emoticon()
{
}

Emoticon::Emoticon(const QString &triggerText, const QString &staticFilePath, const QString &animatedFilePath) :
		TriggerText(triggerText), StaticFilePath(staticFilePath), AnimatedFilePath(animatedFilePath)
{
}

Emoticon::Emoticon(const Emoticon &copyMe)
{
	TriggerText = copyMe.TriggerText;
	StaticFilePath = copyMe.StaticFilePath;
	AnimatedFilePath = copyMe.AnimatedFilePath;
}

Emoticon & Emoticon::operator = (const Emoticon &copyMe)
{
	TriggerText = copyMe.TriggerText;
	StaticFilePath = copyMe.StaticFilePath;
	AnimatedFilePath = copyMe.AnimatedFilePath;

	return *this;
}

bool Emoticon::isNull() const
{
	return TriggerText.isEmpty();
}

QString Emoticon::triggerText() const
{
	return TriggerText;
}

QString Emoticon::staticFilePath() const
{
	return StaticFilePath;
}

QString Emoticon::animatedFilePath() const
{
	return AnimatedFilePath;
}
