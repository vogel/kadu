/*
 * %kadu copyright begin%
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef GADU_ID_VALIDATOR_H
#define GADU_ID_VALIDATOR_H

#include "qt/long-validator.h"

class GaduIdValidator : public LongValidator
{
	Q_OBJECT

	static QValidator *Instance;

	explicit GaduIdValidator(QObject *parent = 0);
	virtual ~GaduIdValidator();

public:
	static void createInstance();
	static void destroyInstance();

	static QValidator * instance();

};

#endif // GADU_ID_VALIDATOR_H
