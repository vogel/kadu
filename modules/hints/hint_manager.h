#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include "configuration/configuration-aware-object.h"
#include "gui/widgets/abstract-tool-tip.h"
#include "gui/windows/main-configuration-window.h"
#include "notify/notifier.h"
#include "hint.h"

class QHBoxLayout;
class QFrame;
class QSpinBox;

class ChatWidget;
class ContactList;
class HintsConfigurationWidget;
class HintOverUserConfigurationWindow;

class HintManager : public Notifier, public ConfigurationUiHandler, public AbstractToolTip, public ConfigurationAwareObject
{
	Q_OBJECT

private:
	QFrame *frame;
	QVBoxLayout *layout;
	QTimer *hint_timer;
	QList<Hint *> hints;
	QFrame *tipFrame;

	double opacity;

	QSpinBox *minimumWidth;
	QSpinBox *maximumWidth;
	QPushButton *configureOverUserHint;
	HintsConfigurationWidget *configurationWidget;
	HintOverUserConfigurationWindow *overUserConfigurationWindow;

	QFrame *overUserConfigurationPreview;
	QLabel *overUserConfigurationIconLabel;
	QLabel *overUserConfigurationTipLabel;

	QMap<QPair<Chat *, QString>, Hint *> linkedHints;

	/**
		ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
	**/
	void setLayoutDirection();

	void processButtonPress(const QString &buttonName, Hint *hint);

	void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, ContactList contacts, const QString &msg);

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

	void minimumWidthChanged(int value);
	void maximumWidthChanged(int value);

	void toolTipClassesHighlighted(const QString &value);

	void hintUpdated();

	void mainConfigurationWindowDestroyed();
	void hintOverUserConfigurationWindowDestroyed();
	void showOverUserConfigurationWindow();
	void updateOverUserPreview();

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

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual void notify(Notification *notification);

	virtual void showToolTip(const QPoint &, Contact);
	virtual void hideToolTip();

	virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);

	void prepareOverUserHint(QFrame *tipFrame, QLabel *iconLabel, QLabel *tipLabel, Contact contact);

};

extern HintManager *hint_manager;

#endif
