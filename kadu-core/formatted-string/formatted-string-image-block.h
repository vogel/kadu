/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef FORMATTED_STRING_IMAGE_BLOCK_H
#define FORMATTED_STRING_IMAGE_BLOCK_H

#include <QtCore/QString>

#include "protocols/services/chat-image.h"

#include "exports.h"

#include "formatted-string.h"

/**
 * @addtogroup FormattedString
 * @{
 */

/**
 * @class FormattedStringImageBlock
 * @short FormattedString subclass that contains information about one image.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class contains information about one image that can be a part of FormattedString.
 */
class KADUAPI FormattedStringImageBlock : public FormattedString
{
	Q_DISABLE_COPY(FormattedStringImageBlock);

	QString ImagePath;
	ChatImage Image;

public:
	/**
	 * @short Create empty FormattedStringImageBlock.
	 */
	FormattedStringImageBlock();

	/**
	 * @short Create FormattedStringImageBlock with image with given path.
	 * @author Rafał 'Vogel' Malinowski
	 * @param imagePath path to image
	 */
	explicit FormattedStringImageBlock(QString imagePath);

	/**
	 * @short Create FormattedStringImageBlock with image with given chat image key.
	 * @author Rafał 'Vogel' Malinowski
	 * @param chatImageKey chat image get of image
	 *
	 * Relative path of image will be generatted automatically from toString() method of passed ChatImage instance. This path can be invalid
	 * if image was not downloaded from peer.
	 */
	explicit FormattedStringImageBlock(ChatImage image);

	virtual ~FormattedStringImageBlock();

	virtual bool operator == (const FormattedString &compareTo);

	virtual void accept(FormattedStringVisitor *visitor) const;

	/**
	 * @short Return true if neither image path nor image key are provided.
	 * @author Rafał 'Vogel' Malinowski
	 * @return true if neither image path nor image key are provided
	 */
	virtual bool isEmpty() const;

	/**
	 * @short Return provided image path.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided image path
	 *
	 * If image path was not provided at construction time then it will be constructed from available chat image key.
	 */
	QString imagePath() const;

	/**
	 * @short Return provided chat image key.
	 * @author Rafał 'Vogel' Malinowski
	 * @return provided chat image key
	 *
	 * If chat image key was not provided at construction time then empty chat image key will be returned.
	 */
	ChatImage image() const;

};

/**
 * @}
 */

#endif // FORMATTED_STRING_IMAGE_BLOCK_H
