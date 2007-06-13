#ifndef DOCKING_H
#define DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

#include "main_configuration_window.h"

/**
 * @defgroup docking Docking
 * @{
 */
class DockingManager : public ConfigurationUiHandler
{
	Q_OBJECT

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;
	void defaultToolTip();

private slots:
	void statusPixmapChanged(const QPixmap &icon, const QString &iconName);
	void changeIcon();
	void dockletChange(int id);
	void pendingMessageAdded();
	void pendingMessageDeleted();

	void configurationUpdated();

public:
	DockingManager();
	virtual ~DockingManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void trayMousePressEvent(QMouseEvent * e);
	QPixmap defaultPixmap();

public slots:
	/**
		Modu³ implementuj±cy dokowanie powinien to ustawic
		na true przy starcie i false przy zamknieciu, aby
		kadu wiedzialo, ze jest zadokowane.
	**/
	void setDocked(bool docked, bool butDontHideOnClose = false);

 	signals:
	void trayPixmapChanged(const QPixmap& pixmap, const QString &name);
	void trayMovieChanged(const QMovie& movie);
	void trayTooltipChanged(const QString& tooltip);
	void searchingForTrayPosition(QPoint& pos);
	void mousePressMidButton();
	void mousePressLeftButton();
	void mousePressRightButton();
};

extern DockingManager* docking_manager;

/** @} */

#endif
