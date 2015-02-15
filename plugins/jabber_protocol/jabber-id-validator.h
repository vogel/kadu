/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_ID_VALIDATOR_H
#define JABBER_ID_VALIDATOR_H

#include <QtGui/QValidator>

class JabberIdValidator : public QValidator
{
	Q_OBJECT

	static QValidator *Instance;

	explicit JabberIdValidator(QObject *parent = 0);
	virtual ~JabberIdValidator();

public:
	static void createInstance();
	static void destroyInstance();

	static QValidator * instance();

	virtual QValidator::State validate(QString &input, int &pos) const;

};

#endif // JABBER_ID_VALIDATOR_H
