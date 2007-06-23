#ifndef DESKTOPDOCK_H
#define DESKTOPDOCK_H

#include <qpixmap.h>
#include <qlabel.h>

#include "main_configuration_window.h"

/* DesktopDockWindow - ikonka dokujaca */

class QSpinBox;

/** @defgroup desktopdock Desktop docking
 * @{
 */

class DesktopDockWindow : public QLabel
{
	Q_OBJECT

	bool isMoving;

protected:
	void mousePressEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
	void updateMask();

public:
	DesktopDockWindow(QWidget *parent, char *name);
	virtual ~DesktopDockWindow();

public slots:
	void startMoving();

signals:
	void dropped(const QPoint &);

};

class DesktopDock : public ConfigurationUiHandler
{
	Q_OBJECT

	DesktopDockWindow *desktopDock;
	QSpinBox *xSpinBox;
	QSpinBox *ySpinBox;

	int menuPos;
	int separatorPos;

private slots:
	void configurationUpdated();

	void setToolTip(const QString &statusText);
	void setPixmap(const QPixmap &DockPixmap, const QString &iconName);
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
