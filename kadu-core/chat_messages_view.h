#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include "kadu_text_browser.h"
#include "userlistelement.h"

class ChatMessage;

class ChatMessagesView : public KaduTextBrowser
{
	Q_OBJECT

	QValueList<ChatMessage *> Messages;
	unsigned int Prune;

	bool CfgNoHeaderRepeat;
	unsigned int CfgHeaderSeparatorHeight;
	unsigned int CfgNoHeaderInterval;
	unsigned int ParagraphSeparator;

	QString ChatSyntaxWithHeader;
	QString ChatSyntaxWithoutHeader;

	bool ScrollLocked;

	QMimeSourceFactory *bodyformat;

	void repaintMessages();
	void updateBackgrounds();
	QString formatMessage(ChatMessage *message, ChatMessage *after);

	void pruneMessages();

private slots:
	void pageUp();
	void pageDown();

	/**
		\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
		TODO: zmieniæ nag³ówek
		TODO; przesun±æ do klasy ChatMessage
		Slot obs³uguj±cy odebranie i zapis obrazka
		\param sender osoba, która wys³a³a obrazek
		\param size rozmiar obrazka
		\param crc32 suma kontrolna obrazka
		\param path ¶cie¿ka do obrazka
	**/
	void imageReceivedAndSaved(UserListElement sender, uint32_t size, uint32_t crc32, const QString& path);

public:
	ChatMessagesView(QWidget *parent = 0, char *name = 0);
	virtual ~ChatMessagesView();

	void appendMessage(ChatMessage *message);
	void appendMessages(QValueList<ChatMessage *> messages);

	void setPrune(unsigned int prune);

public slots:
	void clearMessages();
	void setScrollLocked(bool scrollLocked);

};

#endif // CHAT_MESSAGES_VIEW
