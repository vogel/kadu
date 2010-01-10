#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/abstract-tool-tip.h"
#include "notify/notifier.h"
#include "hint.h"

class QHBoxLayout;
class QFrame;

class ChatWidget;
class BuddyList;
class HintsConfigurationUiHandler;
class HintsConfigurationWidget;
class HintOverUserConfigurationWindow;

class HintManager : public Notifier, public AbstractToolTip, public ConfigurationAwareObject
{
	Q_OBJECT

private:
	QFrame *frame;
	QTimer *hint_timer;
	QFrame *tipFrame;
	QVBoxLayout *layout;
	QString style;
	double opacity;

	QList<Hint *> hints;
	QMap<QPair<Chat , QString>, Hint *> linkedHints;

	HintsConfigurationWidget *configurationWidget;
	
	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, BuddyList buddies, const QString &msg);

	/**
		ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
	**/
	void setLayoutDirection();

	// TODO: usun�� w 0.6
	void realCopyConfiguration(const QString &fromCategory, const QString &fromHint, const QString &toHint);
	void createDefaultConfiguration();

	void import_0_6_5_configuration();

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

	void hintUpdated();

protected:
	virtual void configurationUpdated();

signals:
	/**
		HintManager szuka pozycji traya, aby wy�wietli� w jego okolicy dymki
	**/
	void searchingForTrayPosition(QPoint& pos);

public:
	HintManager(QWidget *parent = 0);
	~HintManager();

	HintsConfigurationUiHandler *uiHandler;

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &, Buddy);
	virtual void hideToolTip();

	virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

	void prepareOverUserHint(QFrame *tipFrame, QLabel *iconLabel, QLabel *tipLabel, Buddy buddy);

	QString Style() { return style; }
	double Opacity() { return opacity; }
	HintsConfigurationUiHandler *UiHandler() { return uiHandler; }
};

extern HintManager *hint_manager;

#endif
