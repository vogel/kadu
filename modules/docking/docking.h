#ifndef DOCKING_H
#define DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>

class DockingManager : public QObject
{
	Q_OBJECT

	private:
		QTimer *icon_timer;
		bool blink;

	private slots:
		void changeIcon();
		void dockletChange(int id);
		void pendingMessageAdded();
		void pendingMessageDeleted();
		void showOffline();
		void showStatus(int status);
		void showCurrentStatus(int status);
		void findTrayPosition(QPoint& pos);

	public:
		DockingManager();
		~DockingManager();
		void trayMousePressEvent(QMouseEvent * e);
		QPixmap defaultPixmap();
		void defaultToolTip();

	signals:
		void trayPixmapChanged(const QPixmap& pixmap);
		void trayTooltipChanged(const QString& tooltip);
		void searchingForTrayPosition(QPoint& pos);
		void mousePressMidButton();
		void mousePressLeftButton();
		void mousePressRightButton();
};

extern DockingManager* docking_manager;

#endif
