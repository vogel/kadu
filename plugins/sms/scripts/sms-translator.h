/*
 * %kadu copyright begin%
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SMS_TRANSLATOR_H
#define SMS_TRANSLATOR_H

#include <QtCore/QObject>
#include <QtScript/QScriptValue>

#include "configuration/configuration-aware-object.h"

/**
 * @addtogroup SMS
 * @{
 */

/**
 * @class SmsTranslator
 * @author Rafał 'Vogel' Malinowski
 * @short Class responsible for providing SMS scripts a translated version of messages.
 */
class SmsTranslator : public QObject
{
	Q_OBJECT

public:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Create new instance of SmsTranslator.
	 * @param parent QObject parent of new instance of SmsTranslator
	 */
	explicit SmsTranslator(QObject *parent = 0);
	virtual ~SmsTranslator();

public slots:
	/**
	 * @author Rafał 'Vogel' Malinowski
	 * @short Translate given text for script.
	 * @param text text to translate
	 * @return translated version of given text
	 */
	QScriptValue tr(const QString &text);

};

/**
 * @}
 */

#endif // SMS_TRANSLATOR_H
