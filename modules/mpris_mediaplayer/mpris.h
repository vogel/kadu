#ifndef MPRIS_H
#define MPRIS_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "configuration_aware_object.h"
#include "main_configuration_window.h"

#include "../mediaplayer/mediaplayer.h"
#include "../mediaplayer/mpris_mediaplayer.h"

class MprisPlayer : public MPRISMediaPlayer, public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT;

	public:
		MprisPlayer(QString name, QString service);
		~MprisPlayer();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	protected:
		virtual void configurationUpdated();
};

extern MprisPlayer* mpris;

#endif
