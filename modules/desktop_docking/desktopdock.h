#ifndef DESKTOPDOCK_H
#define DESKTOPDOCK_H

#include <QPixmap>
#include <QtGui/QLabel>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

/* DesktopDockWindow - ikonka dokujaca */

class QSpinBox;

/** @defgroup desktopdock Desktop docking
 * @{
 */

class DesktopDock : public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	DesktopDockWindow *desktopDock;
	QSpinBox *xSpinBox;
	QSpinBox *ySpinBox;

	int menuPos;
	int separatorPos;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

private slots:
	void setToolTip(const QString &statusText);
	void setPixmap(const QIcon &DockIcon, const QString &iconName);
	void setTrayMovie(const QMovie &movie);
	void findTrayPosition(QPoint &DockPoint);
	void droppedOnDesktop(const QPoint &);
	void updateMenu(bool);

public:
	DesktopDock();
	virtual ~DesktopDock();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

};

extern DesktopDock *desktop_dock;

/** @} */

#endif
