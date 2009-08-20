#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <QtCore/QList>

#include "protocols/protocol.h"

#include "kadu-text-browser.h"

#include "exports.h"

class Chat;
class ChatMessage;
class ChatWidget;
class QResizeEvent;

/**
	Klasa uniwersalnego obiektu wy�wietlaj�cego list� wiadomo�ci(okno chat, historia...).
	\class ChatMessagesView
	\brief Okno przegl�dania listy wiadomo�ci.
**/
class KADUAPI ChatMessagesView : public KaduTextBrowser
{
	Q_OBJECT

	Chat *CurrentChat;
	ChatMessage *PrevMessage;
	QList<ChatMessage *> Messages;

	bool PruneEnabled;

	int LastScrollValue;
	bool LastLine;

	void pruneMessages();

protected:
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	void repaintMessages();

	void scrollToLine();
	/**
		\fn void pageUp()
		Slot przewijaj�cy okno wiadomo�ci o sta�� warto�� w g�r�.
	**/
 	void pageUp();
	/**
		\fn void pageDown()
		Slot przewijaj�cy okno wiadomo�ci o sta�� warto�� w d��.
	**/
 	void pageDown();

	void imageReceived(const QString &imageId, const QString &imagePath);

public:
	ChatMessagesView(Chat *chat = 0, QWidget *parent = 0);
	virtual ~ChatMessagesView();

	/**
		\fn void appendMessage(ChatMessage *message)
		Dodaje pojedyncz� wiadomo�� do danego okna.
		@param message Wiadomo�� do dodania.
	**/
 	void appendMessage(ChatMessage *message);
	/**
		\fn void appendMessages(QList<ChatMessage *> messages)
		Dodaje list� wiadomo�ci do danego okna.
		@param messages Lista wiadomo�ci do dodania.
	**/
 	void appendMessages(QList<ChatMessage *> messages);

	/**
		\fn unsigned int countMessages()
		Zwraca ilo�� wiadomo�ci w danym oknie.
		@see Messages
	**/
 	unsigned int countMessages();

	/**
		\fn void updateBackgrounds()
		Funkcja od�wie�aj�ca kolory t�a wiadomo�ci.
	**/
	void updateBackgroundsAndColors();

	void setPruneEnabled(bool enable) { PruneEnabled = enable; }

	const QList<ChatMessage *> messages() { return Messages; }

	void rememberScrollBarPosition();

	Chat *chat() const { return CurrentChat; }
	void setChat(Chat *chat);

	ChatMessage *&prevMessage() { return PrevMessage; }

public slots:
	/**
		\fn void clearMessages()
		Slot usuwaj�cy wszystkie wiadomo�ci z okna.
	**/
	void clearMessages();

};

#endif // CHAT_MESSAGES_VIEW
