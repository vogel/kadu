#include "misc.h"
#include "kadu.h"
#include "password.h"

remindPassword::remindPassword() {
	fprintf(stderr, "KK remindPassword::remindPassword()\n");
	snr = snw = NULL;
	h = NULL;
}

remindPassword::~remindPassword() {
	fprintf(stderr, "KK remindPassword::~remindPassword()\n");
	
	deleteSocketNotifiers();
	if (h) {
		gg_remind_passwd_free(h);
		h = NULL;
		}
}

void remindPassword::start() {
	fprintf(stderr, "KK remindPassword::start()\n");
	if (!(h = gg_remind_passwd(config.uin, 1))) {
		deleteLater();
		return;
		}

	createSocketNotifiers();
}

void remindPassword::createSocketNotifiers() {
	fprintf(stderr, "KK remindPassword::createSocketNotifiers()\n");

	snr = new QSocketNotifier(h->fd, QSocketNotifier::Read, kadu);
	QObject::connect(snr, SIGNAL(activated(int)), this, SLOT(dataReceived()));
	
	snw = new QSocketNotifier(h->fd, QSocketNotifier::Write, kadu);
	QObject::connect(snw, SIGNAL(activated(int)), this, SLOT(dataSent()));
}

void remindPassword::deleteSocketNotifiers() {
	fprintf(stderr, "KK remindPassword::deleteSocketNotifiers()\n");
	if (snr) {
		snr->setEnabled(false);
		snr->deleteLater();
		snr = NULL;
		}
	if (snw) {
		snw->setEnabled(false);
		snw->deleteLater();
		snw = NULL;
		}
}

void remindPassword::dataReceived() {
	fprintf(stderr, "KK remindPassword::dataReceived()\n");
	if (h->check && GG_CHECK_READ)
		socketEvent();
}

void remindPassword::dataSent() {
	fprintf(stderr, "KK remindPassword::dataSent()\n");
	snw->setEnabled(false);
	if (h->check && GG_CHECK_WRITE)
		socketEvent();
}

void remindPassword::socketEvent() {
	fprintf(stderr, "KK remindPassword::socketEvent()\n");
	if (gg_remind_passwd_watch_fd(h) == -1) {
		deleteSocketNotifiers();
		fprintf(stderr, "KK remindPassword::socketEvent(): error reminding password!\n");
		deleteLater();
		return;
		}
	switch (h->state) {
		case GG_STATE_CONNECTING:
			fprintf(stderr, "KK remindPassword::socketEvent(): changing QSocketNotifiers.\n");
			deleteSocketNotifiers();
			createSocketNotifiers();
		case GG_STATE_ERROR:
			fprintf(stderr, "KK remindPassword::socketEvent(): error reminding password!\n");
			deleteSocketNotifiers();
			deleteLater();
			break;
		case GG_STATE_DONE:
			fprintf(stderr, "KK remindPassword::socketEvent(): success!\n");
			deleteSocketNotifiers();
			deleteLater();
			break;
		default:
			if (h->check && GG_CHECK_WRITE)
				snw->setEnabled(true);
		}
}

#include "password.moc"
