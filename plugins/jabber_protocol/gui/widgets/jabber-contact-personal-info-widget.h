/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004, 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#ifndef JABBER_CONTACT_PERSONAL_INFO_WIDGET_H
#define JABBER_CONTACT_PERSONAL_INFO_WIDGET_H

#include <QtGui/QWidget>

#include "contacts/contact.h"

#include "exports.h"

class QComboBox;
class QLabel;

class JabberContactPersonalInfoWidget : public QWidget
{
	Q_OBJECT

	QLabel *FullNameText;
	QLabel *FamilyNameText;
	QLabel *NicknameText;
	QLabel *BirthdateText;
	QLabel *CityText;
	QLabel *EmailText;
	QLabel *WebsiteText;

	Contact MyContact;
	void createGui();
	void reset();

private slots:
	void personalInfoAvailable(Buddy buddy);
	void urlClicked(const QString &link);

public:
	explicit JabberContactPersonalInfoWidget(Contact &contact, QWidget *parent = 0);
	virtual ~JabberContactPersonalInfoWidget();

};

#endif // JABBER_CONTACT_PERSONAL_INFO_WIDGET_H
