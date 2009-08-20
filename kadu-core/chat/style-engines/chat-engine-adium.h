#ifndef CHAT_ENGINE_ADIUM_H
#define CHAT_ENGINE_ADIUM_H

#include "chat-style-engine.h"

class Chat;
class ChatMessage;
class ChatMessagesView;
class Preview;

class AdiumChatStyleEngine : public ChatStyleEngine
{
	QString jsCode;

	static const char *xhtmlBase;

	QString StyleVariantName;
	QString BaseHref;
	QString HeaderHtml;
	QString FooterHtml;
	QString IncomingHtml;
	QString NextIncomingHtml;
	QString OutgoingHtml;
	QString NextOutgoingHtml;
	QString StatusHtml;

	QString readThemePart(QString part);

	QString replaceKeywords(Chat *chat, QString &styleHref, QString &style);
	QString replaceKeywords(Chat *chat, QString &styleHref, QString &source, ChatMessage *message);

	bool clearDirectory(const QString &directory);

public:
	AdiumChatStyleEngine();

	virtual bool supportVariants() { return true; }
	virtual bool supportEditing() { return false; }
	virtual QString isThemeValid(QString styleName);
	virtual QString currentStyleVariant() { return StyleVariantName; }	

	virtual QStringList styleVariants(QString styleName);

	virtual void clearMessages(ChatMessagesView *view);
	virtual void appendMessages(ChatMessagesView *view, QList<ChatMessage *> messages);
	virtual void appendMessage(ChatMessagesView *view, ChatMessage *message);
	virtual void pruneMessage(ChatMessagesView *view);
	virtual void refreshView(ChatMessagesView *view);

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated() {}

	virtual void loadTheme(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString ) {} //do nothing. Adium styles don't support editing

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_ADIUM_H
