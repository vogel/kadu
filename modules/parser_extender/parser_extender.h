/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef PARSER_EXTENDER_H
#define PARSER_EXTENDER_H

#include <QtCore/QObject>

#include "configuration/configuration-aware-object.h"

class ParserExtender : public ConfigurationAwareObject
{
	void init();
	void close();
	bool isStarted;

protected:
	virtual void configurationUpdated();

public:
	ParserExtender();
	~ParserExtender();
};

extern ParserExtender *parserExtender;

#endif // PARSER_EXTENDER_H
