#include "status/status.h"

#include "mp_status_changer.h"

MediaPlayerStatusChanger::MediaPlayerStatusChanger(QObject *parent) :
		StatusChanger(900, parent), disabled(true), mediaPlayerStatusPosition(DescriptionReplace)
{
}

MediaPlayerStatusChanger::~MediaPlayerStatusChanger()
{
}

void MediaPlayerStatusChanger::changeStatus(StatusContainer *container, Status &status)
{
	Q_UNUSED(container)

	if (disabled || status.isDisconnected())
		return;

	QString description = status.description();
	switch (mediaPlayerStatusPosition)
	{
		case DescriptionReplace:
			description = title;
			break;

		case DescriptionPrepend:
			description = title + description;
			break;

		case DescriptionAppend:
			description = description + title;
			break;

		case PlayerTagReplace:
			if (status.description().indexOf("%player%") > -1)
				description.replace("%player%", title);
			break;
	}

	status.setDescription(description);
}

void MediaPlayerStatusChanger::setTitle(const QString &newTitle)
{
	disabled = false;

	if (newTitle != title)
	{
		title = newTitle;
		emit statusChanged(0);
	}
}

void MediaPlayerStatusChanger::setDisable(bool disable)
{
	disabled = disable;
	emit statusChanged(0);
}

void MediaPlayerStatusChanger::changePositionInStatus(ChangeDescriptionTo newSongInfoPlace)
{
	if (mediaPlayerStatusPosition != newSongInfoPlace)
	{
		mediaPlayerStatusPosition = newSongInfoPlace;
		if (!disabled)
			emit statusChanged(0);
	}
}

#include "moc_mp_status_changer.cpp"
