#ifndef CHAT_H
#define CHAT_H

#include <qdialog.h>
#include <qtextbrowser.h>
#include <qhbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include "misc.h"
#include "userbox.h"
#include "../config.h"

class Chat;

typedef QValueList<Chat*> ChatList;

class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;
		int openPendingMsg(int index,QString& to_add);	

	public:	
		ChatManager();
		/**
			Zwraca liste otwartych okien Chat.
		**/
		const ChatList& chats();
		int registerChat(Chat* chat);
		void unregisterChat(Chat* chat);
		void refreshTitles();
		void refreshTitlesForUin(uin_t uin);
		void changeAppearance();
		void enableEncryptionBtnForUins(UinsList uins);
		Chat* findChatByUins(UinsList uins);
		/**
			Otwiera nowe okno Chat z wymienionymi rozmowcami.
			Parametr time sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
		**/
		int openChat(UinsList senders,time_t time=0);		
		void openPendingMsgs(UinsList uins);
		void openPendingMsgs();
		void sendMessage(uin_t uin,UinsList selected_uins);
		
	public slots:
		void chatMsgReceived(UinsList senders,const QString& msg,time_t time,bool& grab);
		
	signals:
		void chatCreated(const UinsList& senders);
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

	public slots:
		void paste();
		void setAutosend(bool);

	protected:
		void keyPressEvent(QKeyEvent *e);

		bool autosend_enabled;
};

class KaduTextBrowser : public QTextBrowser {
	public:
		KaduTextBrowser(QWidget *parent = 0, const char *name = 0);
		void setSource(const QString &name);
};

/**
	Okno rozmowy
**/
class Chat : public QWidget
{
	Q_OBJECT
	
	private:
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
#ifdef HAVE_OPENSSL
		QPushButton *encryption;
#endif
		QPushButton *sendbtn;
		UserBox *userbox;
		QString myLastMessage;
		int myLastFormatsLength;
		void *myLastFormats;
		int seq;

		void pruneWindow(void);

	private slots:
		void setupEncryptButton(bool enabled);
		void userWhois(void);
		void insertEmoticon(void);
		void changeColor(void);
		void regEncryptSend(void);
		void addMyMessageToHistory(void);
		void clearChatWindow(void);
		void pageUp();
		void pageDown();

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
#ifdef HAVE_OPENSSL
		bool encrypt_enabled;
#endif
		//
		/**
			Rejestruje opcje modulu Chat w oknie konfiguracji.
		**/
		static void initModule();
		Chat(UinsList uins, QWidget *parent = 0, const char *name = 0);
		~Chat();
		void changeAppearance();
		void setTitle(void);
		void formatMessage(bool, const QString &, const QString &, const QString &, QString &);
		void checkPresence(UinsList, const QString &, time_t, QString &);
		void writeMessagesFromHistory(UinsList, time_t);
		void addEmoticon(QString);
		void scrollMessages(QString &);
		void alertNewMessage(void);
		void setEncryptionBtnEnabled(bool);
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
		void hyperlinkClicked(const QString &link);
		void toggledBold(bool on);
		void toggledItalic(bool on);
		void toggledUnderline(bool on);
		void curPosChanged(int para, int pos);
		void specialKeyPressed(int key);
		void colorChanged(const QColor& color);
		void aboutToClose();
		void ackReceivedSlot(int seq);
		
	signals:
		/**
			Sygnal jest emitowany gdy uzytkownik wyda polecenie
			wyslania wiadomosci, np klikajac na guzik "wyslij".
		**/
		void messageSendRequested(Chat* chat);
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
		ColorSelectorButton(QWidget* parent, const QColor& qcolor);

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
	private:
		QValueList<QColor> vl_chatcolor;
		QValueList<QFont> vl_chatfont;
		void updatePreview();
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseEmoticonsStyle(int index);
		void onDefWebBrowser(bool toggled);
		void onUseEncryption(bool toggled);
		void onPruneChat(bool toggled);
		void generateMyKeys(void);
		void chooseColorGet(const QColor& color);
		void chooseColorGet(const QString& text);
		void chooseChatSelect(int nr);
		void chooseChatFont(int nr);
		void chooseChatFontSize(int nr);

};

#endif
