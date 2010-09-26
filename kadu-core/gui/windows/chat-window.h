#ifndef KADU_CHAT_WINDOW_H
#define KADU_CHAT_WINDOW_H

#include "gui/widgets/chat-widget.h"
#include "configuration/configuration-aware-object.h"
#include "os/generic/compositing-aware-object.h"
#include "exports.h"

class QTimer;

class KADUAPI ChatWindow : public QWidget, public ChatContainer, ConfigurationAwareObject, CompositingAwareObject
{
	Q_OBJECT

private:
	ChatWidget *currentChatWidget;
	QTimer *title_timer;  /*!< zmienna przechowuj�ca czas od ostatniego od�wie�enia tytu�u okna */

	void kaduStoreGeometry();
	void kaduRestoreGeometry();

	bool activateWithNewMessages; /*!< czy aktywujemy okno po przyj�ciu nowej wiadomo�ci */
	bool showNewMessagesNum; /*!< czy pokazujemy liczb� nowych wiadomo�ci w tytule nieaktywnego okna */
	bool blinkChatTitle; /*!< czy tytu� nieaktywnego okna z nieprzeczytanymi wiadomo�ciami powinien mruga� */

	void setDefaultGeometry();

private slots:
	void updateTitle();

protected:
	/**
		\fn virtual void closeEvent(QCloseEvent* e)
		Funkcja obs�uguj�ca zamkni�cie okna
	**/
	virtual void closeEvent(QCloseEvent *e);

	/**
		\fn virtual void windowActivationChange(bool oldActive)
		Funkcja steruj�ca mruganiem napisu okna
	**/
	virtual void windowActivationChange(bool oldActive);

	virtual void configurationUpdated();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	ChatWindow(QWidget *parent = 0);
	virtual ~ChatWindow();

	void setChatWidget(ChatWidget *chatWidget);
	ChatWidget * chatWidget();

	void closeChatWidget(ChatWidget *chatWidget);
	void setWindowTitle(const QString &title);

public slots:
	// TODO: rename
	void blinkTitle();
	void showNewMessagesNumInTitle();

	/**
		\fn void alertNewMessage()
		Slot informuj�cy o nowej wiadomo�ci
		mruganie je�li okno jest nieaktywne
	**/
	void alertNewMessage();

signals:
	void chatWidgetActivated(ChatWidget *);

};

#endif
