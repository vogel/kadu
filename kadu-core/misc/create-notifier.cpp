#include "create-notifier.h"

void CreateNotifier::notify(QObject *new_object)
{
	emit objectCreated(new_object);
}