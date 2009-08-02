#ifndef QT4_DOCKING_H
#define QT4_DOCKING_H

#include <QtCore/QObject>
#include <QtGui/QIcon>
#include <QtGui/QSystemTrayIcon>

/**
 * @defgroup qt4_docking QT4 docking
 * @{
 */
class ChatWidget;
class QPoint;

class Qt4TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT

		QMovie *Movie;
		QPoint lastPosition;
	

	private slots:
		void setTrayPixmap(const QIcon &pixmap);
		void setTrayMovie(const QString &movie);
		void setTrayTooltip(const QString &tooltip);
		void findTrayPosition(QPoint &pos);
		void trayActivated(QSystemTrayIcon::ActivationReason reason);

		void movieUpdate();
	public:
		Qt4TrayIcon(QWidget *parent = 0);
		~Qt4TrayIcon();
		
};

extern Qt4TrayIcon* qt4_tray_icon;


/** @} */

#endif
