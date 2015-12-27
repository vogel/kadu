/*
 * %kadu copyright begin%
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CENZOR_H
#define CENZOR_H

#include <QtCore/QObject>

#include "message/message-filter.h"

#include "configuration/cenzor-configuration.h"

class Chat;
class Contact;

class CenzorMessageFilter : public QObject, public MessageFilter
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit CenzorMessageFilter(QObject *parent = nullptr);
	virtual ~CenzorMessageFilter();

	CenzorConfiguration & configuration() { return Configuration; }

protected:
	virtual bool acceptMessage(const Message &message);

private:
	CenzorConfiguration Configuration;

	bool shouldIgnore(const QString &message);
	bool isExclusion(const QString &word);

};

#endif // CENZOR_H
