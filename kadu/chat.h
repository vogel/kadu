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

#include "misc.h"
#include "userbox.h"

class Chat;

typedef QValueList<Chat*> ChatList;

class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;
		int openPendingMsg(int index,QString& to_add);	
		
		struct ChatInfo
		{
			UinsList uins;
			QRect geometry;
			QValueList<int> vertSizes;
			QValueList<int> horizSizes;
		};
		QValueList<ChatInfo> sizes;
		
	public:	
		ChatManager(QObject *parent=NULL, const char *name=NULL);
		~ChatManager();
		void closeAllWindows();
		/**
			Zwraca liste otwartych okien Chat.
		**/
		const ChatList& chats();
		int registerChat(Chat* chat);
		void unregisterChat(Chat* chat);
		void refreshTitles();
		void refreshTitlesForUin(UinType uin);
		void changeAppearance();
		Chat* findChatByUins(UinsList uins);
		/**
			Otwiera nowe okno Chat z wymienionymi rozmowcami.
			Parametr time sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
		**/
		int openChat(UinsList senders,time_t time=0);		
		void openPendingMsgs(UinsList uins);
		void openPendingMsgs();
		void sendMessage(UinType uin,UinsList selected_uins);
		
	public slots:
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);
		
	signals:
		void chatCreated(const UinsList& senders);
		void chatDestroying(const UinsList& senders);
		void chatDestroyed(const UinsList& senders);
};

extern ChatManager* chat_manager;

class EmoticonSelector;
class ColorSelector;

class CustomInput : public QMultiLineEdit {
	Q_OBJECT
	public:
		CustomInput(QWidget *parent = 0, const char *name = 0);

		enum {
			KEY_BOLD,
			KEY_ITALIC,
			KEY_UNDERLINE
			};

	signals:
		void sendMessage();
		void specialKeyPressed(int key);
		/**
			Dowolny przycisk klawiatury zostal nacisniety.
			Przekazany zostaje tak¿e obiekt, który wywo³a³ akcjê - czyli this.
		**/
		void keyPressed(QKeyEvent*, CustomInput *sender);

	public slots:
		void paste();
		void setAutosend(bool);

	protected:
		void keyPressEvent(QKeyEvent *e);

		bool autosend_enabled;
};

class KaduSplitter : public QSplitter
{
	public:
		KaduSplitter(QWidget * parent = 0, const char * name = 0);
		KaduSplitter(Orientation o, QWidget * parent = 0, const char * name = 0 );
	protected:
		QValueList<KaduTextBrowser *> list;
		void drawContents(QPainter *p);
		void childEvent(QChildEvent *c);
};

/**
	Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT

	friend class ChatManager;
	private:
		struct RegisteredButton
		{
			QString name;
			QObject* receiver;
			QString slot;
		};
		static QValueList<RegisteredButton> RegisteredButtons;
		QMap<QString,QPushButton*> Buttons;
	
		UinsList Uins;
		int index;
		int totaloccurences;
		QString title_buffer;
		QTimer *title_timer;  
		QColor actcolor;
		
		EmoticonSelector *emoticon_selector;
		ColorSelector *color_selector;
		QPushButton *boldbtn;
		QPushButton *italicbtn;
		QPushButton *underlinebtn;
		QPushButton *colorbtn;
		QPushButton *iconsel;
		QPushButton *autosend;
		QPushButton *lockscroll;
		QAccel *acc;
		QPushButton *sendbtn;
		UserBox *userbox;
		QString myLastMessage;
		int myLastFormatsLength;
		void *myLastFormats;
		int seq;

		void pruneWindow(void);
		KaduSplitter *vertSplit, *horizSplit;

	private slots:
		void userWhois(void);
		void emoticonSelectorClicked(void);
		void changeColor(void);
		void addMyMessageToHistory(void);
		void clearChatWindow(void);
		void pageUp();
		void pageDown();
		void insertImage();
		void imageReceivedAndSaved(UinType sender,uint32_t size,uint32_t crc32,const QString& path);

	protected:
		void closeEvent(QCloseEvent *);
		QString convertCharacters(QString,bool me);
		virtual void windowActivationChange(bool oldActive);
		void keyPressEvent(QKeyEvent *e);

	public:
		// FIXME - nie powinno byc publicznych zmiennych
		QTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
		//
		/**
			Rejestruje opcje modulu Chat w oknie konfiguracji.
		**/
		static void initModule();
		Chat(UinsList uins, QWidget *parent = 0, const char *name = 0);
		~Chat();
		static void registerButton(const QString& name,QObject* receiver,const QString& slot);
		static void unregisterButton(const QString& name);
		QPushButton* button(const QString& name);
		void changeAppearance();
		void setTitle(void);
		void formatMessage(bool, const QString &, const QString &, const QString &, QString &);
		void checkPresence(UinsList, const QString &, time_t, QString &);
		void writeMessagesFromHistory(UinsList, time_t);
		void addEmoticon(QString);
		void scrollMessages(QString &);
		void alertNewMessage(void);
		/**
			Zwraca liste numerow rozmowcow.
		**/
		const UinsList& uins();

	public slots:
		void HistoryBox(void);
		void sendMessage(void);
		void cancelMessage(void);
		void writeMyMessage(void);
		void changeTitle(void);
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
		void messageFiltering(const UinsList& uins,QCString& msg,bool& stop);
		/**
			Sygnal jest emitowany gdy zakonczy sie proces
			wysylania wiadomosci i zwiazanych z tym czynnosci.
			Oczywiscie nie wiemy czy wiadomosc dotarla.
		**/		
		void messageSent(Chat* chat);
};

class ColorSelectorButton : public QToolButton
{
	Q_OBJECT

	private:
		QColor color;

	private slots:
		void buttonClicked();

	public:
		ColorSelectorButton(QWidget* parent, const QColor& qcolor, const char *name=0);

	signals:
		void clicked(const QColor& color);
};

class ColorSelector : public QWidget
{
	Q_OBJECT
	private slots:
		void iconClicked(const QColor& color);

	public:
		ColorSelector(QWidget* parent = 0, const char *name = 0);
		void alignTo(QWidget* w);

	protected:
		void closeEvent(QCloseEvent *);

	signals:
		void aboutToClose();
		void colorSelect(const QColor&);
};

class ChatSlots :public QObject
{
	Q_OBJECT
	public:
		ChatSlots(QObject *parent=0, const char *name=0);
	private:
		void updatePreview();
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseEmoticonsStyle(int index);
		void onDefWebBrowser(bool toggled);
		void onPruneChat(bool toggled);
		void onFoldLink(bool toggled);
		void chooseColor(const char *name, const QColor &color);
		void chooseFont(const char *name, const QFont &font);
};

#endif
