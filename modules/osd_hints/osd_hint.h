#ifndef OSDHint_H
#define OSDHint_H

#include <QtGui/QBitmap>
#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>

#include "configuration_aware_object.h"
#include "gadu.h"

class QLabel;
class QVBoxLayout;


class Notification;

class OSDHint : public QWidget, ConfigurationAwareObject
{
	Q_OBJECT

	QVBoxLayout *vbox;
	QHBoxLayout *labels;
	QHBoxLayout *callbacksBox;

	QLabel *icon;
	QLabel *label;
	QColor bcolor; //kolor t�a
	QColor fcolor;
	unsigned int secs;
	unsigned int startSecs;

	Notification *notification;
	QStringList details;

	bool haveCallbacks;

	void createLabels(const QPixmap &pixmap);
	void updateText();

	void resetTimeout();

private slots:
	void notificationClosed();

protected:
	virtual void mouseReleaseEvent(QMouseEvent * event);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

	virtual void configurationUpdated();

public:
	OSDHint(QWidget *parent, Notification *notification);
	virtual ~OSDHint();

	void getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);
	bool requireManualClosing();
	bool isDeprecated();

	void addDetail(const QString &detail);

	bool hasUsers() const;
	const UserListElements & getUsers() const;

	Notification * getNotification() { return notification; }

public slots:
	/**
		min�a sekunda, zmniejsza licznik pozosta�ych sekund,
		zwraca true je�eli jeszcze pozosta� czas
		false, gdy czas si� sko�czy�
	**/
	void nextSecond();

	void acceptNotification();
	void discardNotification();

signals:
	void leftButtonClicked(OSDHint *OSDHint);
	void rightButtonClicked(OSDHint *OSDHint);
	void midButtonClicked(OSDHint *OSDHint);
	void closing(OSDHint *OSDHint);
	void updated(OSDHint *OSDHint);
};

#endif
