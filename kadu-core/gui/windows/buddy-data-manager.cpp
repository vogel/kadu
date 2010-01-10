/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "buddy-data-manager.h"

BuddyDataManager::BuddyDataManager(Buddy buddy, QObject *parent) :
		ConfigurationWindowDataManager(parent), MyBuddy(buddy)
{

}

void BuddyDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value) {
	if (section != "Contact")
		return;

	if (name == "display")
		MyBuddy.setDisplay(value.toString());
	else if (name == "firstName")
		MyBuddy.setFirstName(value.toString());
	else if (name == "lastName")
		MyBuddy.setLastName(value.toString());
	else if (name == "familyName")
		MyBuddy.setFamilyName(value.toString());
	else if (name == "city")
		MyBuddy.setCity(value.toString());
	else if (name == "familyCity")
		MyBuddy.setFamilyCity(value.toString());
	else if (name == "nickName")
		MyBuddy.setNickName(value.toString());
	else if (name == "homePhone")
		MyBuddy.setHomePhone(value.toString());
	else if (name == "mobilePhone")
		MyBuddy.setMobile(value.toString());
	else if (name == "email")
		MyBuddy.setEmail(value.toString());
	else if (name == "birthYear")
		MyBuddy.setBirthYear(value.toInt());
	else if (name == "gender")
		MyBuddy.setGender((BuddyGender)value.toInt());
}

QVariant BuddyDataManager::readEntry(const QString &section, const QString &name) {
	if (section != "Contact")
		return QVariant();

	if (name == "display")
		return MyBuddy.display();
	else if (name == "firstName")
		return MyBuddy.firstName();
	else if (name == "lastName")
		return MyBuddy.lastName();
	else if (name == "familyName")
		return MyBuddy.familyName();
	else if (name == "city")
		return MyBuddy.city();
	else if (name == "familyCity")
		return MyBuddy.familyCity();
	else if (name == "nickName")
		return MyBuddy.nickName();
	else if (name == "homePhone")
		return MyBuddy.homePhone();
	else if (name == "mobilePhone")
		return MyBuddy.mobile();
	else if (name == "email")
		return MyBuddy.email();
	else if (name == "birthYear")
		return MyBuddy.birthYear();
	else if (name == "gender")
		return MyBuddy.gender();

	return QVariant();
}
