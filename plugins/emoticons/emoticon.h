/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_H
#define EMOTICON_H

#include <QtCore/QMetaType>
#include <QtCore/QString>

#include "emoticons-exports.h"

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class Emoticon
 * @short Emoticon representation.
 * @author Rafał 'Vogel' Malinowski
 */
class EMOTICONSAPI_TESTS Emoticon
{
	QString TriggerText;
	QString StaticFilePath;
	QString AnimatedFilePath;

public:
	/**
	 * @short Create null emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 */
	Emoticon();

	/**
	 * @short Create emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @param triggerText text that triggers this emoticon to display
	 * @param staticFilePath file path of not-animated version of emoticon
	 * @param animatedFilePath file path of animated version of emoticon
	 */
	Emoticon(const QString &triggerText, const QString &staticFilePath, const QString &animatedFilePath);

	/**
	 * @short Create copy of emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @param copyMe emoticon to copy
	 */
	Emoticon(const Emoticon &copyMe);

	Emoticon & operator = (const Emoticon &copyMe);

	/**
	 * @short Check if emoticon is null.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true, if emoticon is null
	 */
	bool isNull() const;

	/**
	 * @short Return text that triggers this emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @return text that triggers this emoticon
	 */
	QString triggerText() const;

	/**
	 * @short Return file path of not-animated version of emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @return file path of not-animated version of emoticon
	 */
	QString staticFilePath() const;

	/**
	 * @short Return file path of animated version of emoticon.
	 * @author Rafał 'Vogel' Malinowski
	 * @return file path of animated version of emoticon
	 */
	QString animatedFilePath() const;

};

Q_DECLARE_METATYPE(Emoticon);

/**
 * @}
 */

#endif // EMOTICON_H
