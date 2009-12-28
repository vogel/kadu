/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHAT_STYLE_ENGINE_H
#define CHAT_STYLE_ENGINE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

#include <chat/message/message.h>

class HtmlMessagesRenderer;
class MessageRenderInfo;
class Preview;

class ChatStyleEngine
{
protected:
	QString EngineName;
	QString CurrentStyleName;

public:
	virtual ~ChatStyleEngine() {}
	virtual QString engineName() { return EngineName; }
	virtual QString currentStyleName() { return CurrentStyleName; }
	virtual QString currentStyleVariant() { return QString::null; }
	virtual QStringList styleVariants(QString) { return QStringList(); }	

	virtual bool supportVariants() = 0;
	virtual bool supportEditing() = 0;
	virtual QString isThemeValid(QString) = 0; //return QString::null, if theme is not valid
	virtual bool styleUsesTransparencyByDefault(QString) = 0;
	
	virtual void clearMessages(HtmlMessagesRenderer *) = 0;
	virtual void appendMessages(HtmlMessagesRenderer *, QList<MessageRenderInfo *>) = 0;
	virtual void appendMessage(HtmlMessagesRenderer *, MessageRenderInfo *) = 0;
	virtual void pruneMessage(HtmlMessagesRenderer *) = 0;
	virtual void refreshView(HtmlMessagesRenderer *) = 0;
	virtual void messageStatusChanged(HtmlMessagesRenderer *, Message, Message::Status) = 0;

	virtual void prepareStylePreview(Preview *, QString, QString) = 0;

	virtual void configurationUpdated() = 0;

	virtual void loadTheme(const QString &, const QString &) = 0;

	virtual void styleEditionRequested(QString) = 0;

	virtual bool removeStyle(const QString &) = 0;
};

#endif // CHAT_STYLE_ENGINE_H
