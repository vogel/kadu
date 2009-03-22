#ifndef KADU_H
#define KADU_H

#include <QtCore/QEvent>
#include <QtCore/QDateTime>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>

#include "contacts/contact.h"

#include "protocols/protocol.h"
#include "protocols/status.h"

#include "configuration_aware_object.h"
#include "kadu_main_window.h"
#include "exports.h"

class QAction;
class QActionGroup;
class QFile;
class QMenu;
class QMenuBar;
class QPushButton;
class QVBoxLayout;

class Account;
class ActionDescription;
class ContactsListWidget;
class GroupTabBar;
class KaduTextBrowser;

/**
	G��wne okno Kadu
**/
class KADUAPI Kadu : public KaduMainWindow, ConfigurationAwareObject
{
	Q_OBJECT

private slots:
	void connected();
	void connecting();
	void disconnected();
	void imageReceivedAndSaved(UinType sender, quint32 size, quint32 crc32, const QString &path);
	void deleteOldConfigFiles();

	void statusChanged(Account *account, Status status);

protected:
	virtual void customEvent(QEvent *);
	virtual void configurationUpdated();

public:
	Kadu(QWidget *parent = 0);
	~Kadu();

	virtual bool supportsActionType(ActionDescription::ActionType type) {
		return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList | ActionDescription::TypeUser); }
	virtual ContactsListWidget * contactsListWidget() { return 0; }
	virtual ContactList contacts();
	virtual ChatWidget * chatWidget() { return 0; }

	QVBoxLayout * mainLayout() const;

	bool docked() const;

	void setDefaultStatus();
	void startupProcedure();

	static const char *SyntaxText;
	static const char *SyntaxTextNotify;

	const QDateTime &startTime() const;
	void refreshPrivateStatusFromConfigFile();

public slots:
	virtual void show();
	virtual void hide();

	void sendMessage(Contact contact);

	void changeAppearance();
	void blink();
	virtual bool close(bool quit = false);

	void setStatusActionsIcon();

signals:
	void statusPixmapChanged(const QIcon &icon, const QString &icon_name);

	void shown();
	void hiding();

	void searchingForTrayPosition(QPoint &point);
	void settingMainIconBlocked(bool &);

};

class OpenGGChatEvent : public QEvent
{
	int Number;

public:
	OpenGGChatEvent(int num) : QEvent((Type)5432), Number(num) {}
	int number() const { return Number; }

};

void disableContainsSelfUles(KaduAction *action);

extern KADUAPI Kadu *kadu;
extern KADUAPI QMenu *dockMenu;
extern int lockFileHandle;
extern QFile *lockFile;
struct flock;
extern struct flock *lock_str;

#endif
