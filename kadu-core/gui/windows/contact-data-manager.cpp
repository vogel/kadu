/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact-data-manager.h"

ContactDataManager::ContactDataManager(Contact contact, QObject *parent) :
		ConfigurationWindowDataManager(parent), CurrentContact(contact)
{

}

void ContactDataManager::writeEntry(const QString &section, const QString &name, const QVariant &value) {
	if (section != "Contact")
		return;

	if (name == "display")
		CurrentContact.setDisplay(value.toString());
	else if (name == "firstName")
		CurrentContact.setFirstName(value.toString());
	else if (name == "lastName")
		CurrentContact.setLastName(value.toString());
	else if (name == "familyName")
		CurrentContact.setFamilyName(value.toString());
	else if (name == "city")
		CurrentContact.setCity(value.toString());
	else if (name == "familyCity")
		CurrentContact.setFamilyCity(value.toString());
	else if (name == "nickName")
		CurrentContact.setNickName(value.toString());
	else if (name == "homePhone")
		CurrentContact.setHomePhone(value.toString());
	else if (name == "mobilePhone")
		CurrentContact.setMobile(value.toString());
	else if (name == "email")
		CurrentContact.setEmail(value.toString());
	else if (name == "birthYear")
		CurrentContact.setBirthYear(value.toInt());
	else if (name == "gender")
		CurrentContact.setGender((ContactData::ContactGender)value.toInt());
}

QVariant ContactDataManager::readEntry(const QString &section, const QString &name) {
	if (section != "Contact")
		return QVariant();

	if (name == "display")
		return CurrentContact.display();
	else if (name == "firstName")
		return CurrentContact.firstName();
	else if (name == "lastName")
		return CurrentContact.lastName();
	else if (name == "familyName")
		return CurrentContact.familyName();
	else if (name == "city")
		return CurrentContact.city();
	else if (name == "familyCity")
		return CurrentContact.familyCity();
	else if (name == "nickName")
		return CurrentContact.nickName();
	else if (name == "homePhone")
		return CurrentContact.homePhone();
	else if (name == "mobilePhone")
		return CurrentContact.mobile();
	else if (name == "email")
		return CurrentContact.email();
	else if (name == "birthYear")
		return CurrentContact.birthYear();
	else if (name == "gender")
		return CurrentContact.gender();

	return QVariant();
}
