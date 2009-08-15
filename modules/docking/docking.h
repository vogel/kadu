#ifndef DOCKING_H
#define DOCKING_H

#include <QtCore/QMap>
#include <QtGui/QLabel>
#include <QtGui/QIcon>

#include "configuration/configuration-aware-object.h"
#include "status/status-container-aware-object.h"

#include "docking_exports.h"

class QAction;
class QMenu;

class StatusContainer;

/**
 * @defgroup docking Docking
 * @{
 */
class DOCKINGAPI DockingManager : public QObject, ConfigurationAwareObject, StatusContainerAwareObject
{
	Q_OBJECT

	QMenu *DockMenu;
	QAction *CloseKaduAction;
	QAction *containersSeparator;
    #ifdef Q_OS_MAC
	QAction *OpenChatAction;
    #endif

	QMap<StatusContainer *, QAction *> StatusContainerMenus;

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;
	void defaultToolTip();

	void updateContextMenu();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();
	virtual void statusContainerRegistered(StatusContainer *statusContainer);
	virtual void statusContainerUnregistered(StatusContainer *statusContainer);

private slots:
	void statusPixmapChanged(const QIcon &icon);
	void changeIcon();
	void pendingMessageAdded();
	void pendingMessageDeleted();

	void containerStatusChanged();

public:
	DockingManager();
	virtual ~DockingManager();

	void trayMousePressEvent(QMouseEvent * e);
	QIcon defaultPixmap();
	QMenu * dockMenu() { return DockMenu; }

public slots:
	/**
		Modu� implementuj�cy dokowanie powinien to ustawic
		na true przy starcie i false przy zamknieciu, aby
		kadu wiedzialo, ze jest zadokowane.
	**/
	void setDocked(bool docked);

 signals:
	void trayPixmapChanged(const QIcon& icon);
	void trayMovieChanged(const QString& movie);
	void trayTooltipChanged(const QString& tooltip);
	void searchingForTrayPosition(QPoint& pos);
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();
};

extern DOCKINGAPI DockingManager* docking_manager;

/** @} */

#endif
