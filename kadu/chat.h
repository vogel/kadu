#ifndef CHAT_H
#define CHAT_H

#include <qcolor.h>
#include <qdialog.h>
#include <qmap.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qsplitter.h>
#include <qvariant.h>
#include <qvaluelist.h>

#include "emoticons.h"
#include "misc.h"
#include "userbox.h"

class Chat;
class QHBox;

typedef QValueList<Chat*> ChatList;

/**
	Klasa przechowuj±ca informacje o wiadomo¶ci,
	która ma siê pojawiæ w oknie Chat
**/
class ChatMessage
{
	public:
	QString nick;
	QDateTime date;
	QDateTime sdate;
	QString unformattedMessage;
	bool isMyMessage;
	QColor backgroundColor;
	QColor textColor;
	
	/**
		Mo¿e s³u¿yæ do przechowywania informacji o tym
		czy wiadomo¶æ by³a potwierdzona, szyfrowana, ...
		W zamy¶le ma s³u¿yæ do okre¶lania czy jaki¶ obrazek
		ma byæ dodawany do wiadomo¶ci czy nie - jaki obrazek -
		ta informacja bêdzie gdzie indziej
	**/
	QMap<QString, bool> attributes; 
	
	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;
	
	bool needsToBeFormatted;
	
	/**
		Sformatowana wiadomo¶æ (razem z <p> lub <table>)
	**/
	QString message;
	/**
		Konstruktor ustawiaj±cy nick, niesformatowan± wiadomo¶æ,
		czy wiadomo¶æ pochodzi od nas, datê wiadomo¶ci,
		datê nadania wiadomo¶ci przez serwer
	**/
	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	/**
		Konstruktor ustawiaj±cy sformatowan± wiadomo¶æ, kolor t³a i liter wiadomo¶ci
	**/
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};
/**
	Klasa zarz±dzaj±ca oknami Chat
**/
class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;
		int openPendingMsg(int index, ChatMessage &msg);
		
		struct ChatInfo
		{
			UinsList uins;
			QMap<QString, QVariant> map;
		};
		QValueList<ChatInfo> addons;

	public:	
		ChatManager(QObject* parent=NULL, const char* name=NULL);
		~ChatManager();
		/**
			Zwraca liste otwartych okien Chat.
		**/
		const ChatList& chats() const;
		/**
			Zwraca wska¼nik do okna z list± u¿ytkowników uins
			@param uins lista u¿ytkowników
			@return wska¼nik do okna je¶li istnieje w przeciwnym wypadku zwraca NULL
		**/
		Chat* findChatByUins(const UinsList &uins) const;
		/**
			zwraca warto¶æ w³asno¶ci "name" okna okre¶lonego przez uins
			@param uins lista u¿ytkowników identyfikuj±ca okno
			@param name nazwa w³asno¶ci
			@return zwraca warto¶æ w³asno¶ci je¶li okre¶lone okno istnieje,
			je¶li nie to tworzy tak± w³asno¶æ (ustawia na pust±)
		**/
		QVariant& getChatProperty(const UinsList &uins, const QString &name);
		
	public slots:
		/**
			Slot informuj±cy o odebraniu wiadomo¶ci
		**/
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);

		/**
			Otwiera nowe okno Chat z wymienionymi rozmowcami.
			Parametr time sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
		**/
		int openChat(UinsList senders,time_t time=0);		
		/**
			Wpisuje zakolejkowane wiadomo¶ci do okna z u¿ytkownikami "uins"
		**/
		void openPendingMsgs(UinsList uins);
		/**
			Wpisuje wszystkie zakolejkowane wiadomo¶ci do odpowiednich okien
		**/
		void openPendingMsgs();
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)	
		*/
		void sendMessage(UinType uin,UinsList selected_uins);
		/**
			Zamyka wszystkie okna chat
		**/
		void closeAllWindows();
		/**
			Dodaje okno do managera
			@param chat wska¼nik do okna ktore chcemy dodaæ
			@return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);
		/**
			Wyrejestrowuje okno z managera
			Zapisuje w³asno¶ci okna
			wysy³a sygna³ chatDestroying i chatDestroyed
		**/
		void unregisterChat(Chat* chat);
		/**
			Od¶wie¿a tytu³y wszystkich okien
		**/
		void refreshTitles();
		/**
			Od¶wie¿a tytu³y okien które zawieraj± uin
		**/
		void refreshTitlesForUin(UinType uin);
		/**
			Od¶wie¿a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();
		/**
			pozwala przypisaæ okre¶lonemu czatowi (nawet je¿eli on jeszcze nie istnieje)
			pewne w³asno¶ci
		**/
		void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value);
		
	signals:
		/**
		 	Sygna³ ten jest wysy³any po utworzeniu nowego okna chat
			@param senders lista uin-ów z którymi tworzymy nowy chat
		**/
		void chatCreated(const UinsList& senders);
		/**
		 	Sygna³ ten jest wysy³any przed zamnkniêciem okna chat
			@param senders lista uin-ów z które wystêpuj± w tym chacie
		**/
		void chatDestroying(const UinsList& senders);
		/**
		 	Sygna³ ten jest wysy³any po zamnkniêciem okna chat
			@param senders lista uin-ów z które wystêpowa³y w tym chacie
		**/
		void chatDestroyed(const UinsList& senders);
		/**
			Sygna³ ten jest wysy³aniy podczas ka¿dej próby otwarcia nowego okna chcat
			nawet je¶li ju¿ taki istnieje
			@param senders lista uin-ów
		**/
		void chatOpen(const UinsList& senders);
};

extern ChatManager* chat_manager;

class EmoticonSelector;
class ColorSelector;
/**
	Klasa umo¿liwiaj±ca wpisywanie wiadomo¶ci
**/
class CustomInput : public QMultiLineEdit
{
	Q_OBJECT

	protected:
		bool autosend_enabled;
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void keyReleaseEvent(QKeyEvent* e);

	public:
		enum
		{
			KEY_BOLD,
			KEY_ITALIC,
			KEY_UNDERLINE
		};
		CustomInput(QWidget* parent = 0, const char* name = 0);

	public slots:
		void paste();
		/**
			Ustawia/wy³±cza automatyczne wysy³anie wiadomo¶ci
		**/
		void setAutosend(bool);

	signals:
		/**
			Sygna³ zostaje wys³any kiedy naci¶niêto 
			klawisz wys³ania wiadomo¶ci
		**/
		void sendMessage();
		/**
			Sygna³ zostaje wys³any gdy naci¶niêto specjalny klawisz (skrót)
			odpowiadaj±cy np KEY_BOLD
		**/
		void specialKeyPressed(int key);
		/**
			Dowolny przycisk klawiatury zosta³ naci¶niêty.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê - czyli this.
		**/
		void keyPressed(QKeyEvent* e, CustomInput* sender);
		/**
			Dowolny przycisk klawiatury zosta³ zwolniony.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê - czyli this.
		**/
		void keyReleased(QKeyEvent* e, CustomInput* sender);
};

/**
	Klasa wykorzystana do oddzielenia listy u¿ytkowników od historii
	i pola do wpisywania wiadomo¶ci
**/
class KaduSplitter : public QSplitter
{
	protected:
		QValueList<KaduTextBrowser*> textbrowsers;
		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
		KaduSplitter(QWidget* parent = 0, const char* name = 0);
		KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0);
};

class QMimeSourceFactory;
/**
	Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT

	private:
		friend class ChatManager;
		struct RegisteredButton
		{
			QString name;
			QObject* receiver;
			QString slot;
		};
		static QValueList<RegisteredButton> RegisteredButtons;
		QMap<QString,QPushButton*> Buttons;
		
		QValueList<ChatMessage *> ChatMessages;
	
		UinsList Uins;
		int index;
		QString title_buffer;
		QTimer* title_timer;  
		QColor actcolor;
		
		QMimeSourceFactory *bodyformat;
		EmoticonSelector* emoticon_selector;
		ColorSelector* color_selector;
		QPushButton* boldbtn;
		QPushButton* italicbtn;
		QPushButton* underlinebtn;
		QPushButton* colorbtn;
		QPushButton* iconsel;
		QPushButton* autosend;
		QPushButton* lockscroll;
		QAccel* acc;
		QPushButton* sendbtn;
		UserBox* userbox;
		QString myLastMessage;
		int myLastFormatsLength;
		void* myLastFormats;
		int seq;
		KaduSplitter *vertSplit, *horizSplit;
		int ParagraphSeparator;
		QDateTime lastMsgTime;

		void pruneWindow();

	private slots:
		void userWhois();
		void emoticonSelectorClicked();
		void changeColor();
		void addMyMessageToHistory();
		void clearChatWindow();
		void pageUp();
		void pageDown();
		void insertImage();
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);

		void connectAcknowledgeSlots();
		void disconnectAcknowledgeSlots();

	protected:
		virtual void closeEvent(QCloseEvent*);
		QString convertCharacters(QString text, const QColor &bgcolor, EmoticonsStyle style);
		virtual void windowActivationChange(bool oldActive);
		virtual void keyPressEvent(QKeyEvent* e);
		virtual void mouseReleaseEvent(QMouseEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		KaduTextBrowser* body;
		CustomInput* edit;
		QHBox* buttontray;
		/**
			Rejestruje opcje modulu Chat w oknie konfiguracji.
		**/
		static void initModule();
		Chat(UinsList uins, QWidget* parent = 0, const char* name = 0);
		~Chat();
		/**
			Rejestruje przycisk w oknie chat, ustawia funkcje do obs³ugi 
			sygna³u "clicked"
			@param name nazwa przycisku aby moc siê do niego odwo³aæ
			@param receiver obiekt który zawiera funkcje do obs³ugi zdarzenia
			@param slot funkcja która bêdzie obs³ugiwaæ zdarzenie "clicked"
		**/
		static void registerButton(const QString& name, QObject* receiver, const QString& slot);
		/**
			Wyrejestrowuje przycisk ze wszystkich okien
			@param name nazwa przycisku do wyrejestrowania
		**/
		static void unregisterButton(const QString& name);
		/**
			Zwraca wska¼nik do przycisku 
			@param name nazwa przycisku
			@return wska¼nik do przycisku, je¶li przycisk nie istnieje
			zwraca NULL
		**/
		QPushButton* button(const QString& name) const;
		/**
			Formatuje wszystkie wiadomo¶ci na html'a,
			dodaje kolory t³a i czcionek
			@param msgs lista wiadomo¶ci do sformatowania
		**/
		void formatMessages(QValueList<ChatMessage *> &msgs);
		/**
			Formatuje wiadomo¶æ na html'a
			@param msg wiadomo¶æ która bêdzie sformatowana
			@param myBgColor kolor który bêdzie ustawiony jako t³o
			w naszych wiadomo¶ciach
			@param usrBgColor kolor, który bêdzie ustawiony jako t³o
			wiadomo¶ci naszego rozmówcy
			@param myFontColor kolor naszej czcionki
			@param usrFontColor kolor czcionki naszego rozmówcy
			@param style styl ikonek 
		**/
		void formatMessage(ChatMessage &msg,
				QColor myBgColor=QColor(), QColor usrBgColor=QColor(),
				QColor myFontColor=QColor(), QColor usrFontColor=QColor(),
				EmoticonsStyle style=(EmoticonsStyle)config_file.readNumEntry("Chat","EmoticonsStyle"));
		/**
			Dodaje wiadomo¶æ do okna
		**/
		void checkPresence(UinsList, const QString&, time_t, QValueList<ChatMessage *> &messages);
		/**
			Wpisuje do okna historiê rozmowy
			@param senders lista u¿ytkowników identyfikuj±cych okno
			@param time ..
		**/
		void writeMessagesFromHistory(UinsList, time_t);
		/**
			Zwraca listê numerów rozmowców.
		**/
		const UinsList& uins() const;
		/**
			Daje dostêp do wiadomo¶ci aktualnie przechowywanych
			w oknie chat. Metody tej mo¿na u¿yæ do zmiany tre¶ci
			lub w³a¶ciwo¶ci której¶ z wiadomo¶ci w odpowiedzi
			na jakie¶ zdarzenie.
		**/
		QValueList<ChatMessage*>& chatMessages();
		/**
			Od¶wie¿a zawarto¶æ okna uwzglêdniaj±c ewentualne
			zmiany dokonane w której¶ wiadomo¶ci z listy
			uzyskanej za pomoc± metody chatMessages(),
			dodanie nowych wiadomo¶ci lub usuniêcie istniej±cych.
		**/
		void repaintMessages();
		/**
			Zwraca aktualny tytu³ okna
		**/
		const QString& title() const;

	public slots:
		/**
			Slot zmieniaj±cy kolory i czcionki w oknie
		**/
		void changeAppearance();
		/**
			Slot ustawiaj±cy tytu³ okna zgodnie z konfiguracj±
		**/
		void setTitle();
		/**
			Slot dodaje ikonke do pola wpisywania wiadomo¶ci
			@param emot ikonka np. <lol>
		**/
		void addEmoticon(QString);
		/**
			Slot dodaj wiadomo¶ci do okna
			@param messages lista wiadomo¶ci
		**/
		void scrollMessages(const QValueList<ChatMessage *> &);
		/**
			Slot informuj±cy o nowej wiadomo¶ci 
			mruganie je¶li okno jest nieaktywne
		**/
		void alertNewMessage();
		/**
			Slot wywo³ywany po naci¶niêciu przycisku historii
		**/
		void HistoryBox();
		/**
			Slot wywo³ywany po naci¶niêciu przycisku do wysy³ania wiadomo¶ci
		**/
		void sendMessage();
		/**
			Slot wywo³ywany po naci¶nieciu przycisku anulowania
			wysy³aniu wiadomo¶ci
		**/
		void cancelMessage();
		/**
			Slot wpisuj±cy wiadomo¶æ do okna
			@see sendMessage
		**/
		void writeMyMessage();
		/**
		    	Slot zmieniaj±cy tytu³ 
		    	@see setTitle
		**/
		void changeTitle();
		/**
			Slot wywo³ywany przy naci¶niêciu przycisku pogrubienia
		**/
		void toggledBold(bool on);
		/**
			Slot wywo³ywany przy naci¶niêciu przycisku kursywy
		**/
		void toggledItalic(bool on);
		/**
			Slot wywo³ywany przy naci¶niêciu przycisku podkre¶lenia
		**/
		void toggledUnderline(bool on);
		/**
			Slot wywo³ywany kiedy pozycja kursosa zmieni³a siê
		**/
		void curPosChanged(int para, int pos);
		/**
			Slot zostaje wywo³any gdy naci¶niêto specjalny klawisz (skrót)
			odpowiadaj±cy np KEY_BOLD
		**/
		void specialKeyPressed(int key);
		/**
			Slot zostaje wywo³wany gdy u¿ytkownik zmieni³ kolor czcionki
			któr± bêdzie pisa³
		**/
		void colorChanged(const QColor& color);
		/**
			Slot zostaje wywo³any przy zamykaniu okna wyboru ikonek
		**/
		void colorSelectorAboutToClose();
		/**
			Slot zostaje wywo³any gdy nasza wiadomo¶æ zosta³a zablokowana
			przez serwer
		**/
		void messageBlockedSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo³any gdy skrzynka na wiadomo¶ci
			zosta³a zape³niona
		**/
		void messageBoxFullSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo³any gdy wiadomo¶æ nie zosta³a dostarczona
		**/
		void messageNotDeliveredSlot(int seq, UinType uin);
		/**
			Slot zostaje wywo³any gdy wiadomo¶æ zosta³a poprawnie wys³ana
		**/
		void messageAcceptedSlot(int seq, UinType uin);
		
	signals:
		/**
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
		**/
		void messageSendRequested(Chat* chat);
		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
		**/
		void messageFiltering(const UinsList& uins, QCString& msg, bool& stop);
		/**
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
		**/		
		void messageSent(Chat* chat);
};

class ColorSelectorButton : public QPushButton
{
	Q_OBJECT

	private:
		QColor color;

	private slots:
		void buttonClicked();

	public:
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, int width=1, const char *name=0);

	signals:
		/**
			Sygna³ zostaje wys³any po wybraniu koloru
		**/
		void clicked(const QColor& color);
};

class ColorSelector : public QWidget
{
	Q_OBJECT

	private slots:
		void iconClicked(const QColor& color);

	protected:
		void closeEvent(QCloseEvent*);

	public:
		ColorSelector(const QColor &defColor, QWidget* parent = 0, const char* name = 0);

	public slots:
		/**
		  	Slot wyrównuje pozycjê do widgeta
		**/
		void alignTo(QWidget* w);

	signals:
		/**
			Sygna³ jest emitowany gdy zamykany jest wybór kolorów
		**/
		void aboutToClose();
		/**
			Sygna³ jest emitowany gdy wybrano kolor
		**/
		void colorSelect(const QColor&);
};

class ChatSlots :public QObject
{
	Q_OBJECT

	private:
		void updatePreview();
	public:
		ChatSlots(QObject* parent=0, const char* name=0);

	public slots:
		/**
			Slot jest wywo³ywany gdy otwierane jest okno konfiguracji
		**/
		void onCreateConfigDialog();
		/**
			Slot jest wywo³ywany gdy zamykane jest okno konfiguracji
		**/
		void onDestroyConfigDialog();
		/**
			Slot jest wywo³ywany gdy wybierany jest styl ikonek
		**/
		void chooseEmoticonsStyle(int index);
		/**
			Slot jest wywo³ywany gdy wybierane jest czy otwierane okno
			ma byæ pusty czy nie
		**/
		void onPruneChat(bool toggled);
		/**
		**/
		void onFoldLink(bool toggled);
		/**
			Slot jest wywo³ywany gdy wybierany jest maksymalny czas
			blokowania zamkniêcia okna po otrzymaniu wiadomo¶ci
		**/
		void onBlockClose(bool toggled);
		/**
			Slot jest wywo³ywany gdy zmieniono kolor,
			aby uaktualniæ podgl±d 
		**/
		void chooseColor(const char* name, const QColor& color);
		/**
			Slot jest wywo³ywany gdy zmieniono czcionke,
			aby uaktualniæ podgl±d 
		**/
		void chooseFont(const char* name, const QFont& font);
		/**
			Slot jest wywo³ywany gdy wybrano z listy przegl±darkê
		**/
		void findAndSetWebBrowser(int selectedBrowser);
		/**
			Slot jest wywo³ywany gdy aktywowane pole do wpisania opcji
		**/
		void findAndSetBrowserOption(int selectedOption);
		/**
			Slot ustawia przegl±darki wraz z opcjami
		**/
		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		/**
			Slot jest wywo³ywany gdy wybrano z listy przegl±darkê
		**/
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		/**
			Slot ustawia przegl±darki wraz z opcjami
		**/
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};

#endif
