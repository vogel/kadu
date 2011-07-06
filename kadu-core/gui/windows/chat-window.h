#ifndef KADU_CHAT_WINDOW_H
#define KADU_CHAT_WINDOW_H

#include <QtGui/QWidget>

#include "gui/widgets/chat-widget-container.h"
#include "configuration/configuration-aware-object.h"
#include "os/generic/compositing-aware-object.h"
#include "os/generic/desktop-aware-object.h"
#include "exports.h"

class QTimer;

class ChatWidget;

class KADUAPI ChatWindow : public QWidget, public ChatWidgetContainer, ConfigurationAwareObject, CompositingAwareObject, DesktopAwareObject
{
	Q_OBJECT

private:
	ChatWidget *currentChatWidget;
	QTimer *title_timer;  /*!< zmienna przechowująca czas od ostatniego odświeżenia tytułu okna */

	void kaduStoreGeometry();
	void kaduRestoreGeometry();

	bool activateWithNewMessages; /*!< czy aktywujemy okno po przyjęciu nowej wiadomości */
	bool showNewMessagesNum; /*!< czy pokazujemy liczbę nowych wiadomości w tytule nieaktywnego okna */
	bool blinkChatTitle; /*!< czy tytuł nieaktywnego okna z nieprzeczytanymi wiadomościami powinien mrugać */

	void setDefaultGeometry();

private slots:
	void updateIcon();
	void updateTitle();

protected:
	/**
		\fn virtual void closeEvent(QCloseEvent* e)
		Funkcja obsługująca zamknięcie okna
	**/
	virtual void closeEvent(QCloseEvent *e);

	/**
		\fn virtual void changeEvent(QEvent *event)
		Funkcja sterująca mruganiem napisu okna
	**/
	virtual void changeEvent(QEvent *event);

	virtual void configurationUpdated();

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	explicit ChatWindow(ChatWidget *chatWidget, QWidget *parent = 0);
	virtual ~ChatWindow();

	ChatWidget * chatWidget() const { return currentChatWidget; }

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
