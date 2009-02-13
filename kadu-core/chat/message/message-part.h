/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MESSAGE_PART_H
#define MESSAGE_PART_H

#include "protocols/protocol.h"

#include "exports.h"

class QTextDocument;

/**
 * \class MessagePart
 * \brief Part of message that has text and formatting (or an image)
 *
 * This class represents a part of message - text with formatting or an image.
 * Each \see Message is constructed from a series of message parts.
 *
 * Method isImage is used to distinguish image-parts from text-parts.
 *
 * There are two kinds of image parts:
 * <ul>
 *   <li>not-yet-received image parts -
 *       they contains informations about sender of image, and image size and crc32
 *       (used with Gadu-Gadu protocol to assign incoming images with messages).
 *   </li>
 *   <li>received image parts -
 *       they contains only local image path
 *   </li>
 * </ul>
 * @TODO: refactor
 */
class MessagePart
{
	QString Content;
	bool Bold;
	bool Italic;
	bool Underline;
	QColor Color;

	bool Image;
	bool ImageDelayed;
	QString ImagePath;
	QString ImageId;

public:
	static QString loadingImageHtml(const QString &imageId);
	static QString replaceLoadingImages(QString message, const QString &imageId, const QString &imagePath);

	/**
	 * Creates text message part with formatting.
	 * @arg content content of message
	 * @arg bold if true, the whole part is presented with bold font
	 * @arg italic if true, the whole part is presented with italic font
	 * @arg underline if true, the whole part is presented with underline font
	 * @arg color color of whole part
	 */
	MessagePart(const QString &content, bool bold, bool italic, bool underline, QColor color);

	/**
	 * Creates image message part (ready or to-be-received).
	 * @arg imagePath local image path
	 */
	MessagePart(const QString &image, bool delayed);
	virtual ~MessagePart();

	bool isImage() const { return Image; }
	bool isEmpty() const { return !Image && Content.isEmpty(); }

	QString content() const { return Content; }
	bool bold() const { return Bold; }
	bool italic() const { return Italic; }
	bool underline() const { return Underline; }
	QColor color() const { return Color; }

	QString imagePath() const { return ImagePath; }

	/**
	 * Converts message part to HTML - either formatted text or image.
	 * @return HTML representation of message parh.
	 */
	QString toHtml() const;

};

#endif // MESSAGE_PART_H
