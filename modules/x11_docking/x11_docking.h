#ifndef X11_DOCKING_H
#define X11_DOCKING_H

#include <qlabel.h>
#include <qpixmap.h>
#include "gadu.h"

class X11TrayIcon : public QLabel
{
	Q_OBJECT

	private slots:
		void setTrayPixmap(const QPixmap& pixmap, const QString &iconName);
		void setTrayMovie(const QMovie &movie);
		void setTrayTooltip(const QString& tooltip);
		void findTrayPosition(QPoint& pos);
		void chatCreatedSlot(const UserGroup *group);

	protected:
		virtual void enterEvent(QEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);
		virtual bool x11Event(XEvent *);

	public slots:
		void disableTaskbar();
		void enableTaskbar(bool enable=true);
		void tryToDock();

	public:
		X11TrayIcon(QWidget *parent=0, const char *name=0);
		~X11TrayIcon();
};

extern X11TrayIcon* x11_tray_icon;

#endif
