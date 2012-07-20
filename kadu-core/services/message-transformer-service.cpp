/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message/message-transformer.h"

#include "message-transformer-service.h"

MessageTransformerService::MessageTransformerService(QObject *parent) :
		QObject(parent)
{
}

MessageTransformerService::~MessageTransformerService()
{
}

void MessageTransformerService::registerOutgoingMessageTransformer(MessageTransformer *transformer)
{
	if (!transformer || OutgoingMessageTransformers.contains(transformer))
		return;

	OutgoingMessageTransformers.append(transformer);
}

void MessageTransformerService::unregisterOutgoingMessageTransformer(MessageTransformer *transformer)
{
	OutgoingMessageTransformers.removeAll(transformer);
}

QString MessageTransformerService::transformOutgoingMessage(const Chat &chat, const QString &message)
{
	QString result = message;
	foreach (MessageTransformer *transformer, OutgoingMessageTransformers)
		result = transformer->transformMessage(chat, result);
	return result;
}

void MessageTransformerService::registerIncomingMessageTransformer(MessageTransformer *transformer)
{
	if (!transformer || IncomingMessageTransformers.contains(transformer))
		return;

	IncomingMessageTransformers.append(transformer);
}

void MessageTransformerService::unregisterIncomingMessageTransformer(MessageTransformer *transformer)
{
	IncomingMessageTransformers.removeAll(transformer);
}

QString MessageTransformerService::transformIncomingMessage(const Chat &chat, const QString &message)
{
	QString result = message;
	foreach (MessageTransformer *transformer, IncomingMessageTransformers)
		result = transformer->transformMessage(chat, result);
	return result;
}
