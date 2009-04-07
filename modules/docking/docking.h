#ifndef DOCKING_H
#define DOCKING_H

#include <QtGui/QLabel>
#include <QtGui/QIcon>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

#include "docking_exports.h"

/**
 * @defgroup docking Docking
 * @{
 */
class DOCKINGAPI DockingManager : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	enum IconType {BlinkingEnvelope = 0, StaticEnvelope = 1, AnimatedEnvelope = 2} newMessageIcon;
	QTimer *icon_timer;
	bool blink;
	void defaultToolTip();

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

private slots:
	void statusPixmapChanged(const QIcon &icon, const QString &iconName);
	void changeIcon();
	void dockletChange(int id);
	void pendingMessageAdded();
	void pendingMessageDeleted();

public:
	DockingManager();
	virtual ~DockingManager();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void trayMousePressEvent(QMouseEvent * e);
	QIcon defaultPixmap();

public slots:
	/**
		Modu� implementuj�cy dokowanie powinien to ustawic
		na true przy starcie i false przy zamknieciu, aby
		kadu wiedzialo, ze jest zadokowane.
	**/
	void setDocked(bool docked);

 signals:
	void trayPixmapChanged(const QIcon& icon, const QString &name);
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
