#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include "configuration_aware_object.h"
#include "../modules/gadu_protocol/gadu.h"
#include "osd_hint.h"
#include "main_configuration_window.h"
#include "userbox.h"

#include "../notify/notify.h"

class ChatWidget;

class QSpinBox;

class HintsConfigurationWidget;

class OSDHintManager : public Notifier, public ConfigurationUiHandler, public ToolTipClass, ConfigurationAwareObject
{
	Q_OBJECT

private:
	QFrame *frame;
	QVBoxLayout *layout;
	QTimer *hint_timer;
	QList<OSDHint *> hints;
	QFrame *tipFrame;
	double opacity;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	QWidget *overUserSyntax;
	HintsConfigurationWidget *configurationWidget;

	QMap<QPair<UserListElements, QString>, OSDHint *> linkedHints;

	/**
		ustala róg, od którego bêdzie liczona pozycja grupy dymków
	**/
	void setLayoutDirection();

	void processButtonPress(const QString &buttonName, OSDHint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, UserListElements senders, const QString &msg);

	// TODO: usun±æ w 0.6
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
	void leftButtonSlot(OSDHint *hint);

	/**
		na dymku o numerze id klikniêto prawym przyciskiem myszy
		w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
	**/
	void rightButtonSlot(OSDHint *hint);

	/**
		na dymku o numerze id klikniêto ¶rodkowym przyciskiem myszy
		w zale¿no¶ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s± wszystkie
	**/
	void midButtonSlot(OSDHint *hint);

	void deleteHint(OSDHint *hint);
	void deleteHintAndUpdate(OSDHint *hint);

	void notificationClosed(Notification *notification);

	/**
		Pokazuje dymek zwi±zany z notyfikacj±.
	**/
	OSDHint *addHint(Notification *notification);

	/**
		je¿eli dymek dotyczy³ konkrentej osoby lub grupy osób, to otwierane jest okno chatu
		dymek jest kasowany
	**/
	void openChat(OSDHint *hint);

	void chatWidgetActivated(ChatWidget *chat);

	/**
		usuwa wszystkie dymki
	**/
	void deleteAllHints();

	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);

	void toolTipClassesHighlighted(const QString &value);

	void hintUpdated();

protected:
	virtual void configurationUpdated();

signals:
	/**
		OSDHintManager szuka pozycji traya, aby wy¶wietliæ w jego okolicy dymki
	**/
	void searchingForTrayPosition(QPoint& pos);

public:
	OSDHintManager(QWidget *parent=0, const char *name=0);
	~OSDHintManager();

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &, const UserListElement &);
	virtual void hideToolTip();

	virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

};

extern OSDHintManager *hint_manager;

#endif
