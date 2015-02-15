/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "misc/error.h"
#include "debug.h"

#include "server/gadu-connection.h"
#include "server/gadu-writable-session-token.h"
#include "gadu-account-details.h"

#include "gadu-chat-image-service.h"

GaduChatImageService::GaduChatImageService(Account account, QObject *parent) :
		ChatImageService(account, parent), ReceiveImages(false)
{
}

GaduChatImageService::~GaduChatImageService()
{
}

void GaduChatImageService::setConnection(GaduConnection *connection)
{
	Connection = connection;
}

void GaduChatImageService::setGaduChatService(GaduChatService *gaduChatService)
{
	if (CurrentChatService)
		disconnect(CurrentChatService.data(), 0, this, 0);

	CurrentChatService = gaduChatService;
	if (CurrentChatService)
		connect(CurrentChatService.data(), SIGNAL(chatImageKeyReceived(QString,ChatImage)),
		        this, SLOT(chatImageKeyReceivedSlot(QString,ChatImage)));
}

void GaduChatImageService::setReceiveImages(bool receiveImages)
{
	ReceiveImages = receiveImages;
}

void GaduChatImageService::handleEventImageRequest(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(e->event.image_request.sender).arg(e->event.image_request.size).arg(e->event.image_request.crc32)));

	if (!Connection || !Connection.data()->hasSession())
		return;

	auto image = chatImageFromSizeCrc32(e->event.image_request.size, e->event.image_request.crc32);
	if (!ChatImages.contains(image))
		return;

	QByteArray content = ChatImages.value(image);
	if (content.isEmpty())
		return;

	auto writableSessionToken = Connection.data()->writableSessionToken();
	gg_image_reply(writableSessionToken.rawSession(), e->event.image_request.sender, image.key().toUtf8().constData(),
			content.constData(), content.length());
}

void GaduChatImageService::handleEventImageReply(struct gg_event *e)
{
	kdebugm(KDEBUG_INFO, "%s", qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
			.arg(e->event.image_reply.sender).arg(e->event.image_reply.size)
			.arg(e->event.image_reply.crc32).arg(e->event.image_reply.filename)));

	auto image = chatImageFromSizeCrc32(e->event.image_reply.size, e->event.image_reply.crc32);
	QByteArray imageData(e->event.image_reply.image, e->event.image_reply.size);

	if (image.isNull() || imageData.isEmpty())
		return;

	emit chatImageAvailable(image, imageData);
}

ChatImage GaduChatImageService::chatImageFromSizeCrc32(quint32 size, quint32 crc32) const
{
	auto key = (static_cast<uint64_t>(crc32) << 32) | size;
	auto stringKey = QString{"%1"}.arg(key, 16, 16);

	return {stringKey, size};
}

void GaduChatImageService::chatImageKeyReceivedSlot(const QString &id, const ChatImage &chatImage)
{
	if (ReceiveImages)
		emit chatImageKeyReceived(id, chatImage);
}

void GaduChatImageService::requestChatImage(const QString &id, const ChatImage &chatImage)
{
	if (!Connection || !Connection.data()->hasSession())
		return;

	if (id.isEmpty() || chatImage.key().length() != 16)
		return;

	bool ok;
	auto imageKey = chatImage.key().toULongLong(&ok, 16);
	if (!ok)
		return;

	auto crc32 = static_cast<uint32_t>(imageKey >> 32);
	auto size = static_cast<uint32_t>(imageKey & 0x0000FFFF);

	auto writableSessionToken = Connection.data()->writableSessionToken();
	gg_image_request(writableSessionToken.rawSession(), id.toUInt(), size, crc32);
}

ChatImage GaduChatImageService::prepareImageToBeSent(const QByteArray &imageData)
{
	quint32 crc32 = imageData.isEmpty() ? 0 : gg_crc32(0, (const unsigned char*)imageData.constData(), imageData.length());

	auto result = chatImageFromSizeCrc32(imageData.size(), crc32);
	ChatImages.insert(result, imageData);
	return result;
}

Error GaduChatImageService::checkImageSize(qint64 size) const
{
	GaduAccountDetails *details = dynamic_cast<GaduAccountDetails *>(account().details());
	if (!details || !details->chatImageSizeWarning() || size <= RECOMMENDED_MAXIMUM_SIZE)
		return Error(NoError, QString());

	QString message = tr("This image has %1 KiB and exceeds recommended maximum size of %2 KiB. Some clients may have trouble with too large images.")
		+ '\n' + tr("Do you really want to send this image?");
	message = message.arg((size + 1023) / 1024).arg(RECOMMENDED_MAXIMUM_SIZE / 1024);

	return Error(ErrorLow, message);
}

#include "moc_gadu-chat-image-service.cpp"
