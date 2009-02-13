#ifndef CHAT_ENGINE_KADU_H
#define CHAT_ENGINE_KADU_H

#include "chat_style_engine.h"

class ChatMessage;
class ChatMessagesView;
class Preview;

class KaduChatStyleEngine : public ChatStyleEngine
{
	QString ChatSyntaxWithHeader; /*!< Chat syntax with header */
	QString ChatSyntaxWithoutHeader; /*!< Chat syntax without header */

	QString formatMessage(ChatMessage *message, ChatMessage *after);
	void repaintMessages(ChatMessagesView *view);
public:
	KaduChatStyleEngine();
	virtual bool supportVariants() { return false; }
	virtual bool supportEditing() { return true; }
	virtual bool isThemeValid(QString styleName);
	
	virtual void clearMessages(ChatMessagesView *view);
	virtual void appendMessages(ChatMessagesView *view, QList<ChatMessage *> messages);
	virtual void appendMessage(ChatMessagesView *view, ChatMessage *message);
	virtual void pruneMessage(ChatMessagesView *view) {};
	virtual void refreshView(ChatMessagesView *view);

	virtual void createStylePreview(Preview *preview, QString &styleName) {};

	virtual void configurationUpdated();

	virtual void loadTheme(QString &styleName);
//TODO 0.6.6:
	virtual void styleEditionRequested(QString &styleName) {};

};

#endif // CHAT_ENGINE_KADU_H
