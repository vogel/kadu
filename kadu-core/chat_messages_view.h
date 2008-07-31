#ifndef CHAT_MESSAGES_VIEW
#define CHAT_MESSAGES_VIEW

#include <QtCore/QList>

#include "configuration_aware_object.h"
#include "kadu_text_browser.h"
#include "protocol.h"

#include "exports.h"

class ChatMessage;
class QResizeEvent;

/**
	Klasa uniwersalnego obiektu wy�wietlaj�cego list� wiadomo�ci(okno chat, historia...).
	\class ChatMessagesView
	\brief Okno przegl�dania listy wiadomo�ci.
**/
class KADUAPI ChatMessagesView : public KaduTextBrowser, ConfigurationAwareObject
{
	Q_OBJECT

	int lastScrollValue;
	bool lastLine;

	QList<ChatMessage *> Messages; /*!< Lista wiadomo�ci wy�wietlanych w danym obiekcie. */
	unsigned int Prune; /*!< Maksymalna ilo�� wy�wietlanych wiadomo�ci. */
 
	bool CfgNoHeaderRepeat; /*!< Usuwanie powtarzaj�cych si� nag��wk�w wiadomo�ci. */
	unsigned int CfgHeaderSeparatorHeight; /*!< Wysoko�� separatora nag��wk�w. */
	unsigned int CfgNoHeaderInterval; /*!< Odst�p czasu, przy jakim b�d� usuwane nag��wki. */
	unsigned int ParagraphSeparator; /*!< Wysoko�� separatora wiadomo�ci. */
 
	bool NoServerTime; /*!< Usuwanie czasu serwera z wiadomo�ci. */
	int NoServerTimeDiff; /*!< Maksymalna r��nica czasu serwera i lokalnego, dla kt�rej czas serwera b�dzie usuwany. */
 
	QString ChatSyntaxWithHeader; /*!< Sk�adnia stylu rozmowy z nag��wkiem. */
	QString ChatSyntaxWithoutHeader; /*!< Sk�adnia stylu rozmowy bez nag��wka. */
 
	/**
		\fn void repaintMessages()
		Funkcja od�wie�aj�ca widok wiadomo�ci.
	**/
	void repaintMessages(); 
	/**
		\fn void updateBackgrounds()
		Funkcja od�wie�aj�ca kolory t�a wiadomo�ci.
	**/
	void updateBackgrounds();
	/**
		\fn QString formatMessage(ChatMessage *message, ChatMessage *after)
		Funkcja formatuj�ca wiadomo�ci wed�ug ustawie� w konfiguracji.
		@param message wiadomo�� do sformatowania
		@param after wtf?
	**/
 	QString formatMessage(ChatMessage *message, ChatMessage *after);

	/**
		\fn void pruneMessages()
		Funkcja ograniczaj�ca ilo�� wy�wietlanych wiadomo�ci do maksymalnej ustawionej w konfiguracji.
		@see Prune
	**/
	void pruneMessages();

protected:
	virtual void configurationUpdated();
	virtual void resizeEvent(QResizeEvent *e);

private slots:

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

	/**
		\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
		TODO: zmieni� nag��wek
		TODO; przesun�� do klasy ChatMessage
		Slot obs�uguj�cy odebranie i zapis obrazka
		\param sender osoba, kt�ra wys�a�a obrazek
		\param size rozmiar obrazka
		\param crc32 suma kontrolna obrazka
		\param path �cie�ka do obrazka
	**/
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString& path);

public:
	ChatMessagesView(QWidget *parent = 0);
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
		\fn void setPrune(unsigned int prune)
		Ustawia maksymaln� ilo�� wiadomo�ci w danym oknie.
		@param prune Maksymalna ilo�� wiadomo�ci.
		@see Prune
	**/
	void setPrune(unsigned int prune);

public slots:
	/**
		\fn void clearMessages()
		Slot usuwaj�cy wszystkie wiadomo�ci z okna.
	**/
	void clearMessages();

};

#endif // CHAT_MESSAGES_VIEW
