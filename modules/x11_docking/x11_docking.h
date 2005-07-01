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
		void setTrayTooltip(const QString& tooltip);
		void findTrayPosition(QPoint& pos);
		void chatCreatedSlot(const UinsList& senders);

	protected:
		virtual void enterEvent(QEvent* e);
		virtual void mousePressEvent(QMouseEvent* e);

	public slots:
		void disableTaskbar();
		void enableTaskbar(bool enable=true);

	public:
		X11TrayIcon(QWidget *parent=0, const char *name=0);
		~X11TrayIcon();
		void show();
};

extern X11TrayIcon* x11_tray_icon;

#endif
