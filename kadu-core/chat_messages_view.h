#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <QList>

#include "configuration_aware_object.h"
#include "kadu_text_browser.h"
#include "protocol.h"

class ChatMessage;
class QResizeEvent;

/**
	Klasa uniwersalnego obiektu wy¶wietlaj±cego listê wiadomo¶ci(okno chat, historia...).
	\class ChatMessagesView
	\brief Okno przegl±dania listy wiadomo¶ci.
**/
class ChatMessagesView : public KaduTextBrowser, ConfigurationAwareObject
{
	Q_OBJECT

	QList<ChatMessage *> Messages; /*!< Lista wiadomo¶ci wy¶wietlanych w danym obiekcie. */
	unsigned int Prune; /*!< Maksymalna ilo¶æ wy¶wietlanych wiadomo¶ci. */
 
	bool CfgNoHeaderRepeat; /*!< Usuwanie powtarzaj±cych siê nag³ówków wiadomo¶ci. */
	unsigned int CfgHeaderSeparatorHeight; /*!< Wysoko¶æ separatora nag³ówków. */
	unsigned int CfgNoHeaderInterval; /*!< Odstêp czasu, przy jakim bêd± usuwane nag³ówki. */
	unsigned int ParagraphSeparator; /*!< Wysoko¶æ separatora wiadomo¶ci. */
 
	bool NoServerTime; /*!< Usuwanie czasu serwera z wiadomo¶ci. */
	int NoServerTimeDiff; /*!< Maksymalna ró¿nica czasu serwera i lokalnego, dla której czas serwera bêdzie usuwany. */
 
	QString ChatSyntaxWithHeader; /*!< Sk³adnia stylu rozmowy z nag³ówkiem. */
	QString ChatSyntaxWithoutHeader; /*!< Sk³adnia stylu rozmowy bez nag³ówka. */
 
/// 	Q3MimeSourceFactory *bodyformat;
 
	/**
		\fn void repaintMessages()
		Funkcja od¶wie¿aj±ca widok wiadomo¶ci.
	**/
	void repaintMessages(); 
	/**
		\fn void updateBackgrounds()
		Funkcja od¶wie¿aj±ca kolory t³a wiadomo¶ci.
	**/
	void updateBackgrounds();
	/**
		\fn QString formatMessage(ChatMessage *message, ChatMessage *after)
		Funkcja formatuj±ca wiadomo¶ci wed³ug ustawieñ w konfiguracji.
		@param message wiadomo¶æ do sformatowania
		@param after wtf?
	**/
 	QString formatMessage(ChatMessage *message, ChatMessage *after);
	/**
		\fn void pruneMessages()
		Funkcja ograniczaj±ca ilo¶æ wy¶wietlanych wiadomo¶ci do maksymalnej ustawionej w konfiguracji.
		@see Prune
	**/
	void pruneMessages();

protected:
	virtual void configurationUpdated();
	virtual void resizeEvent(QResizeEvent *e);

private slots:
	/**
		\fn void pageUp()
		Slot przewijaj±cy okno wiadomo¶ci o sta³± warto¶æ w górê.
	**/
 	void pageUp();
	/**
		\fn void pageDown()
		Slot przewijaj±cy okno wiadomo¶ci o sta³± warto¶æ w dó³.
	**/
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
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString& path);

public:
	ChatMessagesView(QWidget *parent = 0);
	virtual ~ChatMessagesView();

	/**
		\fn void appendMessage(ChatMessage *message)
		Dodaje pojedyncz± wiadomo¶æ do danego okna.
		@param message Wiadomo¶æ do dodania.
	**/
 	void appendMessage(ChatMessage *message);
	/**
		\fn void appendMessages(QList<ChatMessage *> messages)
		Dodaje listê wiadomo¶ci do danego okna.
		@param messages Lista wiadomo¶ci do dodania.
	**/
 	void appendMessages(QList<ChatMessage *> messages);

	/**
		\fn unsigned int countMessages()
		Zwraca ilo¶æ wiadomo¶ci w danym oknie.
		@see Messages
	**/
 	unsigned int countMessages();

	/**
		\fn void setPrune(unsigned int prune)
		Ustawia maksymaln± ilo¶æ wiadomo¶ci w danym oknie.
		@param prune Maksymalna ilo¶æ wiadomo¶ci.
		@see Prune
	**/
	void setPrune(unsigned int prune);

public slots:
	/**
		\fn void clearMessages()
		Slot usuwaj±cy wszystkie wiadomo¶ci z okna.
	**/
	void clearMessages();

};

#endif // CHAT_MESSAGES_VIEW
