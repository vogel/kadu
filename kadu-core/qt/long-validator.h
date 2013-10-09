/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2005, 2006 Marcin Ślusarz (joi@kadu.net)
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

#ifndef LONG_VALIDATOR_H
#define LONG_VALIDATOR_H

#include <QtGui/QValidator>
#include "exports.h"

class KADUAPI LongValidator : public QValidator
{
	Q_OBJECT
	Q_DISABLE_COPY(LongValidator)

	qlonglong Bottom;
	qlonglong Top;

public:
	LongValidator(qlonglong bottom, qlonglong top, QObject *parent = 0);
	virtual ~LongValidator();

	virtual QValidator::State validate(QString &input, int &) const;
	virtual void fixup(QString &input) const;

};

#endif // LONG_VALIDATOR_H
