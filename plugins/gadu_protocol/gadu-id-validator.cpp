/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gadu-id-validator.h"

QValidator *GaduIdValidator::Instance = 0;

void GaduIdValidator::createInstance()
{
	if (!Instance)
		Instance = new GaduIdValidator();
}

void GaduIdValidator::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

QValidator * GaduIdValidator::instance()
{
	return Instance;
}

GaduIdValidator::GaduIdValidator(QObject *parent) :
		LongValidator(1LL, 3999999999LL, parent)
{
}

GaduIdValidator::~GaduIdValidator()
{
}

#include "moc_gadu-id-validator.cpp"
