#ifndef CHAT_H
#define CHAT_H

#include <qdialog.h>
#include <qtextbrowser.h>
#include <qhbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>
#include <qsplitter.h>
#include <qrect.h>
#include <qcolor.h>
#include <qvariant.h>
#include <qmap.h>

#include "misc.h"
#include "userbox.h"

class Chat;

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
	
	//mo¿e s³u¿yæ do przechowywania informacji o tym
	//czy wiadomo¶æ by³a potwierdzona, szyfrowana, ...
	//w zamy¶le ma s³u¿yæ do okre¶lania czy jaki¶ obrazek ma byæ dodawany
	//do wiadomo¶ci czy nie - jaki obrazek - ta informacja bêdzie gdzie indziej
	QMap<QString, bool> attributes; 
	
	//inne atrybuty?
	//QMap<QString, QString> stringAttributes;
	
	bool needsToBeFormatted;
	
	//sformatowana wiadomo¶æ (razem z <p> lub <table>)
	QString message;

	ChatMessage(const QString &nick, const QString &unformattedMessage, bool myMessage, QDateTime date, QDateTime sdate=QDateTime());
	ChatMessage(const QString &formattedMessage, const QColor &bgColor=Qt::white, const QColor &textColor=Qt::black);
};

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
		const ChatList& chats();
		Chat* findChatByUins(UinsList uins);
		QVariant& getChatProperty(const UinsList &uins, const QString &name);
		
	public slots:
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);

		/**
			Otwiera nowe okno Chat z wymienionymi rozmowcami.
			Parametr time sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
		**/
		int openChat(UinsList senders,time_t time=0);		

		void openPendingMsgs(UinsList uins);
		void openPendingMsgs();
		void sendMessage(UinType uin,UinsList selected_uins);
		void closeAllWindows();

		int registerChat(Chat* chat);
		void unregisterChat(Chat* chat);

		void refreshTitles();
		void refreshTitlesForUin(UinType uin);
		void changeAppearance();
		/**
			pozwala przypisaæ okre¶lonemu czatowi (nawet je¿eli on jeszcze nie istnieje)
			pewne w³asno¶ci
		**/
		void setChatProperty(const UinsList &uins, const QString &name, const QVariant &value);
		
	signals:
		void chatCreated(const UinsList& senders);
		void chatDestroying(const UinsList& senders);
		void chatDestroyed(const UinsList& senders);
};

extern ChatManager* chat_manager;

class EmoticonSelector;
class ColorSelector;

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
		void setAutosend(bool);

	signals:
		void sendMessage();
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

class KaduSplitter : public QSplitter
{
	protected:
		QValueList<KaduTextBrowser*> list;
		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
		KaduSplitter(QWidget* parent = 0, const char* name = 0);
		KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0);
};

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
		
		QValueList<ChatMessage *> chatMessages;
	
		UinsList Uins;
		int index;
		QString title_buffer;
		QTimer* title_timer;  
		QColor actcolor;
		
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

	protected:
		virtual void closeEvent(QCloseEvent*);
		QString convertCharacters(QString,bool me);
		virtual void windowActivationChange(bool oldActive);
		virtual void keyPressEvent(QKeyEvent* e);
		virtual void mouseReleaseEvent(QMouseEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		QTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
		/**
			Rejestruje opcje modulu Chat w oknie konfiguracji.
		**/
		static void initModule();
		Chat(UinsList uins, QWidget* parent = 0, const char* name = 0);
		~Chat();
		static void registerButton(const QString& name, QObject* receiver, const QString& slot);
		static void unregisterButton(const QString& name);
		QPushButton* button(const QString& name);
		void formatMessage(ChatMessage &message);
		void checkPresence(UinsList, const QString&, time_t, QValueList<ChatMessage *> &messages);
		void writeMessagesFromHistory(UinsList, time_t);
		/**
			Zwraca liste numerow rozmowcow.
		**/
		const UinsList& uins();

	public slots:
		void changeAppearance();
		void setTitle();
		void addEmoticon(QString);
		void scrollMessages(const QValueList<ChatMessage *> &);
		void alertNewMessage();

		void HistoryBox();
		void sendMessage();
		void cancelMessage();
		void writeMyMessage();
		void changeTitle();
		void toggledBold(bool on);
		void toggledItalic(bool on);
		void toggledUnderline(bool on);
		void curPosChanged(int para, int pos);
		void specialKeyPressed(int key);
		void colorChanged(const QColor& color);
		void colorSelectorAboutToClose();
		void ackReceivedSlot(int seq);
		
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
		void alignTo(QWidget* w);

	signals:
		void aboutToClose();
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
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseEmoticonsStyle(int index);
		void onPruneChat(bool toggled);
		void onFoldLink(bool toggled);
		void chooseColor(const char* name, const QColor& color);
		void chooseFont(const char* name, const QFont& font);

		void findAndSetWebBrowser(int selectedBrowser);
		void findAndSetBrowserOption(int selectedOption);

		static void initBrowserOptions(QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		static void findBrowser(int selectedBrowser, QComboBox *browserCombo, QComboBox *browserOptionsCombo, QLineEdit *browserPath);
		static void setBrowserOption(int selectedOption, QLineEdit *browserPathEdit, int chosenBrowser);
};

#endif
