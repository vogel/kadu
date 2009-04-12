#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include "configuration_aware_object.h"
#include "gui/widgets/abstract-tool-tip.h"
#include "hint.h"
#include "main_configuration_window.h"
#include "notify/notify.h"

class QSpinBox;

class ChatWidget;
class HintsConfigurationWidget;

class HintManager : public Notifier, public ConfigurationUiHandler, public AbstractToolTip, public ConfigurationAwareObject
{
	Q_OBJECT

private:
	QFrame *frame;
	QVBoxLayout *layout;
	QTimer *hint_timer;
	QList<Hint *> hints;
	QFrame *tipFrame;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	QWidget *overUserSyntax;
	HintsConfigurationWidget *configurationWidget;

	QMap<QPair<ContactList *, QString>, Hint *> linkedHints;

	/**
		ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
	**/
	void setLayoutDirection();

	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, ContactList contacts, const QString &msg);

	// TODO: usun�� w 0.6
	void realCopyConfiguration(const QString &fromHint, const QString &toHint);
	void createDefaultConfiguration();

private slots:
	/**
		min��a sekunda: aktualizuje czasy wszystkich dymk�w
		usuwa te, kt�rym min�� czas
	**/
	void oneSecond();

	/**
		przesuwa ca�� grup� hint�w zgodnie z preferowanym rozmiarem grupy
		i wykryt�/okre�lon� w konfiguracji pozycj� traya
	**/
	void setHint();

	/**
		na dymku o numerze id klikni�to lewym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void leftButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikni�to prawym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void rightButtonSlot(Hint *hint);

	/**
		na dymku o numerze id klikni�to �rodkowym przyciskiem myszy
		w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
	**/
	void midButtonSlot(Hint *hint);

	void deleteHint(Hint *hint);
	void deleteHintAndUpdate(Hint *hint);

	void notificationClosed(Notification *notification);

	/**
		Pokazuje dymek zwi�zany z notyfikacj�.
	**/
	Hint *addHint(Notification *notification);

	/**
		je�eli dymek dotyczy� konkrentej osoby lub grupy os�b, to otwierane jest okno chatu
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

	void toolTipClassesHighlighted(const QString &value);

	void hintUpdated();

protected:
	virtual void configurationUpdated();

signals:
	/**
		HintManager szuka pozycji traya, aby wy�wietli� w jego okolicy dymki
	**/
	void searchingForTrayPosition(QPoint& pos);

public:
	HintManager(QWidget *parent=0, const char *name=0);
	~HintManager();

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &, Contact);
	virtual void hideToolTip();

	virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0, char *name = 0);

};

extern HintManager *hint_manager;

#endif
