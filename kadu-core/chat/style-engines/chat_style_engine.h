#ifndef CHAT_STYLE_ENGINE_H
#define CHAT_STYLE_ENGINE_H

#include <QtCore/QString>
#include <QtCore/QStringList>

class ChatMessage;
class ChatMessagesView;
class Preview;

class ChatStyleEngine
{
protected:
	QString EngineName;
	QString CurrentStyleName;
public:
	virtual QString engineName() { return EngineName; }
	virtual QString currentStyleName() { return CurrentStyleName; }
	virtual QString currentStyleVariant() { return QString::null; }
	virtual QStringList styleVariants(QString) { return QStringList(); }	

	virtual bool supportVariants() = 0;
	virtual bool supportEditing() = 0;
	virtual QString isThemeValid(QString) = 0; //return QString::null, if theme is not valid
	
	virtual void clearMessages(ChatMessagesView *) = 0;
	virtual void appendMessages(ChatMessagesView *, QList<ChatMessage *>) = 0;
	virtual void appendMessage(ChatMessagesView *, ChatMessage *) = 0;
	virtual void pruneMessage(ChatMessagesView *) = 0;
	virtual void refreshView(ChatMessagesView *) = 0;

	virtual void prepareStylePreview(Preview *, QString, QString) = 0;

	virtual void configurationUpdated() = 0;

	virtual void loadTheme(const QString &, const QString &) = 0;

	virtual void styleEditionRequested(QString) = 0;

	virtual bool removeStyle(const QString &) = 0;
};

#endif // CHAT_STYLE_ENGINE_H
