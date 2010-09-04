#ifndef MPRIS_H
#define MPRIS_H

#include <QtCore/QString>

#include "gui/windows/main-configuration-window.h"

#include "../mediaplayer/mediaplayer.h"
#include "../mediaplayer/mpris_mediaplayer.h"

class MprisPlayer : public MPRISMediaPlayer, public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT

	public:
		MprisPlayer(QString name, QString service);
		~MprisPlayer();
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	protected:
		virtual void configurationUpdated();
};

extern MprisPlayer* mpris;

#endif
