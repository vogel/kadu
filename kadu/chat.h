#ifndef CHAT_H
#define CHAT_H

#include <qdialog.h>
#include <qtextbrowser.h>
#include <qhbox.h>
#include <qmultilineedit.h>
#include <qpushbutton.h>
#include <qarray.h>
#include <qtoolbutton.h>
#include <qvaluelist.h>

#include "misc.h"
#include "userbox.h"
#include "../config.h"

extern QValueList<UinsList> wasFirstMsgs;

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
class Chat : public QWidget {
	Q_OBJECT
		int index;
		int totaloccurences;
		UinsList uins;
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
		QPushButton *cancelbtn;
		UserBox *userbox;
		QString myLastMessage;
		int myLastFormatsLength;
		void *myLastFormats;
		int seq, acks;

		void pruneWindow(void);

	public:
		Chat(UinsList uins, QWidget *parent = 0, const char *name = 0);
		~Chat();
		void changeAppearance();
		void setTitle(void);
		void formatMessage(bool, const QString &, const QString &, const QString &, QString &);
		void checkPresence(UinsList, QString &, time_t, QString &);
		void writeMessagesFromHistory(UinsList, time_t);
		void addEmoticon(QString);
		void scrollMessages(QString &);
		void alertNewMessage(void);
		void setEncryptionBtnEnabled(bool);		
		/**
			rejestruje opcje modulu Chat w oknie konfiguracji
		**/
		static void initModule();

		QTextBrowser *body;
		CustomInput *edit;
		QHBox *buttontray;
#ifdef HAVE_OPENSSL
		bool encrypt_enabled;
#endif
    
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

	protected:
		void closeEvent(QCloseEvent *);
		QString convertCharacters(QString,bool me);
		virtual void windowActivationChange(bool oldActive);
		void keyPressEvent(QKeyEvent *e);

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

	private:
		QString title_buffer;
		QTimer *title_timer;  
		QColor actcolor;
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
	
	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void chooseEmoticonsStyle(int index);
		void onDefWebBrowser(bool toggled);
		void onUseEncryption(bool toggled);
		void onPruneChat(bool toggled);
		void generateMyKeys(void);

};

#endif
