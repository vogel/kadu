#ifndef PASSWORD_H
#define PASSWORD_H

#include <qobject.h>
#include <qsocketnotifier.h>

class remindPassword : public QObject {
	Q_OBJECT

	public:
		remindPassword();
		~remindPassword();

	private:
		void deleteSocketNotifiers();
		void createSocketNotifiers();

		struct gg_http *h;
		struct gg_pubdir *p;

		QSocketNotifier *snr;
		QSocketNotifier *snw;

	public slots:
		void start();

	private slots:
		void socketEvent();
		void dataReceived();
		void dataSent();
};

#endif
