#ifndef CHAT_ENGINE_KADU_H
#define CHAT_ENGINE_KADU_H

#include <QtCore/QObject>

#include "chat-style-engine.h"

class ChatMessage;
class ChatMessagesView;
class Preview;
class SyntaxList;

class KaduChatStyleEngine : public QObject, public ChatStyleEngine
{
	Q_OBJECT

	SyntaxList *syntaxList;

	QString ChatSyntaxWithHeader; /*!< Chat syntax with header */
	QString ChatSyntaxWithoutHeader; /*!< Chat syntax without header */

	QString formatMessage(ChatMessage *message, ChatMessage *after);
	void repaintMessages(ChatMessagesView *view);
private slots:
	void chatSyntaxFixup(QString &syntax);
	void chatFixup(QString &syntax);
	void validateStyleName(const QString &name, bool &valid);
	void syntaxAdded(const QString &syntaxName);

public:
	KaduChatStyleEngine();
	~KaduChatStyleEngine();
	virtual bool supportVariants() { return false; }
	virtual bool supportEditing() { return true; }
	virtual QString isThemeValid(QString styleName);
	
	virtual void clearMessages(ChatMessagesView *view);
	virtual void appendMessages(ChatMessagesView *view, QList<ChatMessage *> messages);
	virtual void appendMessage(ChatMessagesView *view, ChatMessage *message);
	virtual void pruneMessage(ChatMessagesView *view) {};
	virtual void refreshView(ChatMessagesView *view);

	virtual void prepareStylePreview(Preview *preview, QString styleName, QString variantName);

	virtual void configurationUpdated();

	virtual void loadTheme(const QString &styleName, const QString &variantName);

	virtual void styleEditionRequested(QString styleName);

	virtual bool removeStyle(const QString &styleName);
};

#endif // CHAT_ENGINE_KADU_H
