#ifndef KADU_CHAT_WIDGET_H
#define KADU_CHAT_WIDGET_H

#include <qdatetime.h>
#include <qwidget.h>
#include <qhbox.h>


#include <qvaluelist.h>

#include "config_file.h"
#include "custom_input.h"
#include "emoticons.h"
#include "protocol.h"
#include "usergroup.h"
#include "toolbar.h"

class QAccel;
class QHBox;
class QMimeSourceFactory;
class QObject;
class QPushButton;

class ChatWidget;
class ChatManager;
class ChatMessage;
class ColorSelector;
class EmoticonSelector;
class KaduSplitter;
class KaduTextBrowser;
class UserBox;

/** \typedef QValueList<ChatWidget*> ChatList */
typedef QValueList<ChatWidget*> ChatList;

/**
	Klasa tworz±ca okno rozmowy, rejestruj±ca przyciski,
	formatuj±ca wiadomo¶æ itp.
	\class ChatWidget
	\brief Okno rozmowy
**/

class ChatWidget : public QHBox
{
	Q_OBJECT

private:
	friend class ChatManager;

	QValueList<ChatMessage *> ChatMessages; /*!< wiadomo¶ci wpisane w oknie rozmowy */
	QString Caption; /*!< tytu³ okna */
	Protocol *CurrentProtocol;
	UserGroup *Users; /*!< lista u¿ytkowników w danym oknie */
	int index;	/*!< nr okna (z chat menad¿era) */
	QColor actcolor; /*!< zmienna przechowuj±ca aktualny kolor */
	QPixmap pix;
	CustomInput* Edit; /*!< okno do wpisywania wiadomo¶ci */
	QMimeSourceFactory *bodyformat; /*!< zmienna ustawiaj±ca format */
	EmoticonSelector* emoticon_selector; /*!< okienko do wyboru emotikonek */
	ColorSelector* color_selector; /*!< okienko do wyboru koloru */
	bool AutoSend; /*!< okre¶la czy Return powoduje wys³anie wiadomo¶ci */
	bool ScrollLocked; /*!< blokuje przewijanie okna rozmowy */
	bool WaitingForACK;
	UserBox* userbox; /*!< lista kontaktów przydatna gdy jeste¶my w konferencji */
	QString myLastMessage; /*!< zmienna przechowuj±ca nasz± ostatni± wiadomo¶æ */
	KaduSplitter *vertSplit, *horizSplit; /*!< obiekty oddzielaj±ce kontrolki od siebie */
	int ParagraphSeparator; /*!< odstêp miêdzy kolejnymi wypowiedziami */

	QDateTime lastMsgTime; /*!< czas ostatniej wiadomo¶ci */
	//pomijanie nag³ówków w wiadomo¶ciach
	UserListElement PreviousSender;           //pamiêtamy od kogo by³a ostatnia wiadomo¶æ
	bool CfgNoHeaderRepeat; /*!< okre¶la czy u¿ywamy usuwania nag³ówków */
	int CfgHeaderSeparatorHeight; /*!< wysoko¶æ separatora nag³ówków */
	int CfgNoHeaderInterval; /*!< interwa³ po jakim przywracany jest nag³ówek */

	QString ChatSyntaxWithHeader;
	QString ChatSyntaxWithoutHeader;
	time_t LastTime;

	/**
		\fn void pruneMessages()
		Funkcja czyszcz±ca okno rozmowy
	**/
	void pruneMessages();

	KaduTextBrowser* body; /*!< historia rozmowy, proszê NIE dawaæ dostêpu na zewn±trz do tej zmiennej */

	// TODO: remove
	int activationCount;

	unsigned int NewMessagesCount; /*!< liczba nowych wiadomo¶ci w oknie rozmowy */

private slots:
	/**
		\fn void changeColor()
		Slot zmieniaj±cy kolor czcionki
	**/
	void changeColor(const QWidget* activating_widget);

	/**
		\fn void pageUp()
		Slot przewijaj±cy historiê rozmowy w górê
	**/
	void pageUp();

	/**
		\fn void pageDown()
		Slot przewijaj±cy historiê rozmowy w dó³
	**/
	void pageDown();

	/**
		\fn void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path)
		TODO: zmieniæ nag³ówek
		Slot obs³uguj±cy odebranie i zapis obrazka
		\param sender osoba, która wys³a³a obrazek
		\param size rozmiar obrazka
		\param crc32 suma kontrolna obrazka
		\param path ¶cie¿ka do obrazka
	**/
	void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString& path);

	/**
		\fn void connectAcknowledgeSlots();
		Slot pod³±czaj±cy potwierdzenie dostarczenia wiadomo¶ci
	**/
	void connectAcknowledgeSlots();

	/**
		\fn void disconnectAcknowledgeSlots();
		Slot od³±czaj±cy potwierdzenie dostarczenia wiadomo¶ci
	**/
	void disconnectAcknowledgeSlots();

	void selectedUsersNeeded(const UserGroup*& user_group);

	void sendActionAddedToToolbar(ToolButton*, ToolBar*);

protected:
	virtual void keyPressEvent(QKeyEvent* e);
	bool keyPressEventHandled(QKeyEvent *);

public:
	/**
		Konstruktor okna rozmowy
		\fn Chat(UserListElements usrs, QWidget* parent = 0, const char* name = 0)
		\param usrs lista kontaktów, z którymi prowadzona jest rozmowa
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	ChatWidget(Protocol *initialProtocol, const UserListElements &usrs, QWidget* parent = 0, const char* name = 0);

	/**
		\fn ~Chat()
		Destruktor okna rozmowy
	**/
	~ChatWidget();

	/**
		\fn void newMessage(const QString &protocolName, UserListElements senders, const QString &msg, time_t time)
		Dodaje wiadomo¶æ do okna
		\param protocolName nazwa protoko³u
		\param senders lista u¿ytkowników
		\param msg wiadomo¶c
		\param time czas
		**/
	void newMessage(const QString &protocolName, UserListElements senders, const QString &msg, time_t time);

	/**
		\fn const UserGroup *users() const
		Zwraca listê numerów rozmowców.
	**/
	const UserGroup *users() const;

	/**
		\fn QValueList<ChatMessage*>& chatMessages()
		Daje dostêp do wiadomo¶ci aktualnie przechowywanych
		w oknie chat. Metody tej mo¿na u¿yæ do zmiany tre¶ci
		lub w³a¶ciwo¶ci której¶ z wiadomo¶ci w odpowiedzi
		na jakie¶ zdarzenie.
	**/
	QValueList<ChatMessage*>& chatMessages();

	/**
		\fn void repaintMessages()
		Od¶wie¿a zawarto¶æ okna uwzglêdniaj±c ewentualne
		zmiany dokonane w której¶ wiadomo¶ci z listy
		uzyskanej za pomoc± metody chatMessages(),
		dodanie nowych wiadomo¶ci lub usuniêcie istniej±cych.
	**/
	void repaintMessages();

	/**
		\fn const QString& title() const
		Zwraca aktualny tytu³ okna
	**/
	const QString& caption() const;

	/**
		\fn CustonInput* edit()
		Zwraca wska¼nik do okna wpisywania wiadomo¶ci
	**/
	CustomInput* edit();

	/**
		\fn bool autoSend() const
		Zwraca warto¶æ okre¶laj±c±, czy Return powoduje
		wys³anie wiadomo¶ci.
	**/
	bool autoSend() const;

	void setLastMessage(const QString &msg);

	bool waitingForACK() const;

	virtual void dragEnterEvent(QDragEnterEvent *e);
	virtual void dropEvent(QDropEvent *e);
	virtual void dragMoveEvent(QDragMoveEvent *e);

	void scrollMessagesToBottom();
	virtual bool eventFilter(QObject *watched, QEvent *e);

	Protocol *currentProtocol();

	unsigned int newMessagesCount() const;

	QDateTime getLastMsgTime();
	QPixmap icon();

	void storeGeometry();
	void restoreGeometry();

public slots:
	/**
		\fn void changeAppearance()
		Slot zmieniaj±cy kolory i czcionki w oknie
	**/
	void changeAppearance();

	/**
		\fn void refreshTitle()
		Slot ustawiaj±cy tytu³ okna zgodnie z konfiguracj±
	**/
	void refreshTitle();

	/**
		\fn void addEmoticon(QString)
		Slot dodaje ikonke do pola wpisywania wiadomo¶ci
		\param emot ikonka np. <lol>
	**/
	void addEmoticon(QString);

	void messageNotDeliveredSlot(const QString &message);

	void messageAcceptedSlot();

	/**
		\fn void editTextChanged()
		Slot jest wywo³ywany, gdy zmieni³a siê zawarto¶æ
		pola edycji wiadomo¶ci. Ustawia poprawny stan przycisku
		akcji sendAction w oknie rozmowy.
	**/
	void editTextChanged();

	/**
		\fn void scrollMessages(const QValueList<ChatMessage *> &)
		Slot dodaj wiadomo¶ci do okna
		\param messages lista wiadomo¶ci
	**/
	void scrollMessages(const QValueList<ChatMessage *> &);

	/**
		\fn void sendMessage()
		Slot wywo³ywany po naci¶niêciu przycisku
		do wysy³ania wiadomo¶ci
	**/
	void sendMessage();

	/**
		\fn void cancelMessage()
		Slot wywo³ywany po naci¶nieciu przycisku anulowania
		wysy³aniu wiadomo¶ci
	**/
	void cancelMessage();

	/**
		\fn void writeMyMessage()
		Slot wpisuj±cy wiadomo¶æ do okna
		\see sendMessage
	**/
	void writeMyMessage();

	/**
		\fn void curPosChanged(int para, int pos)
		Slot wywo³ywany kiedy pozycja kursosa zmieni³a siê
		\param  para nie obslugiwane
		\param pos nie obslugiwane
	**/
	void curPosChanged(int para, int pos);

	/**
		\fn void specialKeyPressed(int key)
		Slot zostaje wywo³any gdy naci¶niêto specjalny klawisz (skrót)
		odpowiadaj±cy np KEY_BOLD
		\param key warto¶æ naci¶niêtego specjalnego klawisza
	**/
	void specialKeyPressed(int key);

	/**
		\fn void colorChanged(const QColor& color)
		Slot zostaje wywo³wany gdy u¿ytkownik zmieni³ kolor czcionki
		któr± bêdzie pisa³
		\param color warto¶æ zmienionego koloru
	**/
	void colorChanged(const QColor& color);

	/**
		\fn void colorSelectorAboutToClose()
		Slot zostaje wywo³any przy zamykaniu okna wyboru ikonek
	**/
	void colorSelectorAboutToClose();

	/**
		\fn void clearChatWindow()
		Slot czyszcz±cy okno rozmowy
	**/
	void clearChatWindow();

	/**
		\fn void setAutoSend(bool auto_send)
		Ustaw autowysylanie
	**/
	void setAutoSend(bool auto_send);

	/**
		\fn void setScrollLocked(bool locked)
		Ustaw blokade przewijania
	**/
	void setScrollLocked(bool locked);

	/**
		\fn void emoticonSelectorClicked()
		Slot wy¶wietlaj±cy okno z wyborem emotikonek
	**/
	void openEmoticonSelector(const QWidget* activating_widget);

	/**
		\fn void insertImage()
		Slot wklejaj±cy obrazek do okna rozmowy (wybór obrazka)
	**/
	void insertImage();

	void makeActive();
	void markAllMessagesRead();

signals:
	/**
		\fn void messageSendRequested(Chat* chat)
		Sygnal jest emitowany gdy uzytkownik wyda polecenie
		wyslania wiadomosci, np klikajac na guzik "wyslij".
		\param chat wska¼nik do okna które emituje sygna³
	**/
	void messageSendRequested(ChatWidget* chat);

	/**
		\fn void messageSent(Chat* chat)
		Sygnal jest emitowany gdy zakonczy sie proces
		wysylania wiadomosci i zwiazanych z tym czynnosci.
		Oczywiscie nie wiemy czy wiadomosc dotarla.
		\param chat wska¼nik do okna rozmowy,
		 które emitowa³o sygna³
	**/
	void messageSent(ChatWidget* chat);

	/**
		\fn void messageSentAndConfirmed(UserListElements receivers, const QString& message)
		This signal is emited when message was sent
		and it was confirmed.
		When confirmations are turned off signal is
		emited immediately after message was send
		just like messageSent() signal.
		\param receivers list of receivers
		\param message the message
	**/
	void messageSentAndConfirmed(UserListElements receivers, const QString& message);

	/**
		\fn void fileDropped(const UserGroup *users, const QString& fileName)
		Sygna³ jest emitowany, gdy w oknie Chat
		upuszczono plik.
	\param users lista u¿ytkowników
		\param fileName nazwa pliku
	**/
	void fileDropped(const UserGroup *users, const QString &fileName);

	void messageReceived(ChatWidget *);

	void captionUpdated();
	void closed();

};

#endif
