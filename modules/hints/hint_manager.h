#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include <qcolor.h>
#include <qfont.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpair.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qtimer.h>

#include "configuration_aware_object.h"
#include "gadu.h"
#include "hint.h"
#include "main_configuration_window.h"
#include "userbox.h"

#include "../notify/notify.h"

class ChatWidget;

class QSpinBox;

class HintManager : public Notifier, public ConfigurationUiHandler, public ToolTipClass, ConfigurationAwareObject
{
	Q_OBJECT

private:
	QFrame *frame;
	QVBoxLayout *layout;
	QTimer *hint_timer;
	QPtrList<Hint> hints;
	QFrame *tipFrame;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;

	QMap<QPair<UserListElements, QString>, Hint *> linkedHints;

	/**
		ustala róg, od którego bêdzie liczona pozycja grupy dymków
	**/
	void setLayoutDirection();

	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, UserListElements senders, const QString &msg);

	// TODO: usun±æ w 0.6
	void realCopyConfiguration(const QString &fromHint, const QString &toHint);
	void import_0_5_0_Configuration();
	void import_0_5_0_Configuration_fromTo(const QString &from, const QString &to,
		const QString &syntax = QString::null, const QString &detailSyntax = QString::null);
	void createDefaultConfiguration();

private slots:
	/**
		minê³a sekunda: aktualizuje czasy wszystkich dymków
		usuwa te, którym min±³ czas
	**/
	void oneSecond();

	/**
		przesuwa ca³± grupê hintów zgodnie z preferowanym rozmiarem grupy
		i wykryt±/okre¶lon± w konfiguracji pozycj± traya
	**/
	void setHint();

	/**
		na dymku o numerze id klikniêto lewym przyciskiem myszy
		w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
	**/
	void leftButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikniêto prawym przyciskiem myszy
		w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
	**/
	void rightButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikniêto ¶rodkowym przyciskiem myszy
		w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
	**/
	void midButtonSlot(Hint *hint);

	void deleteHint(Hint *hint);
	void deleteHintAndUpdate(Hint *hint);

	void notificationClosed(Notification *notification);

	/**
		Pokazuje dymek zwi±zany z notyfikacj±.
	**/
	Hint *addHint(Notification *notification);

	/**
		je¿eli dymek dotyczy³ konkrentej osoby lub grupy osób, to otwierane jest okno chatu
		dymek jest kasowany
	**/
	void openChat(Hint *hint);

	void chatWidgetActivated(ChatWidget *chat);

	/**
		usuwa wszystkie dymki
	**/
	void deleteAllHints();

	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);

protected:
	virtual void configurationUpdated();

signals:
	/**
		HintManager szuka pozycji traya, aby wy¶wietliæ w jego okolicy dymki
	**/
	void searchingForTrayPosition(QPoint& pos);

public:
	HintManager(QWidget *parent=0, const char *name=0);
	~HintManager();

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &, const UserListElement &);
	virtual void hideToolTip();

	virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

};

extern HintManager *hint_manager;

#endif
