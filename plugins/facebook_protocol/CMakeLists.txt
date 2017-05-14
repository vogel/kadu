project (facebook_protocol)

cmake_minimum_required (VERSION 2.8)

find_package (Kadu REQUIRED CONFIG)

kadu_api_directories (plugins/facebook_protocol
	.
)

set (SOURCES
	qfacebook/http/qfacebook-download-contacts-delta-job.cpp
	qfacebook/http/qfacebook-download-contacts-job.cpp
	qfacebook/http/qfacebook-download-threads-job.cpp
	qfacebook/http/qfacebook-download-unread-messages-job.cpp
	qfacebook/http/qfacebook-download-unread-threads-job.cpp
	qfacebook/http/qfacebook-http-api.cpp
	qfacebook/http/qfacebook-http-reply.cpp
	qfacebook/http/qfacebook-http-request.cpp
	qfacebook/http/qfacebook-login-job.cpp

	qfacebook/messages/qfacebook-connect-ack.cpp
	qfacebook/messages/qfacebook-connect.cpp
	qfacebook/messages/qfacebook-disconnect.cpp
	qfacebook/messages/qfacebook-ping.cpp
	qfacebook/messages/qfacebook-pong.cpp
	qfacebook/messages/qfacebook-publish-ack.cpp
	qfacebook/messages/qfacebook-publish-recorded.cpp
	qfacebook/messages/qfacebook-publish.cpp
	qfacebook/messages/qfacebook-subscribe.cpp
	qfacebook/messages/qfacebook-unsubscribe.cpp

	qfacebook/mqtt/qfacebook-mqtt-api.cpp
	qfacebook/mqtt/qfacebook-mqtt-connection.cpp

	qfacebook/publish/qfacebook-publish-foreground-state.cpp
	qfacebook/publish/qfacebook-publish-inbox.cpp
	qfacebook/publish/qfacebook-publish-mark-thread.cpp
	qfacebook/publish/qfacebook-publish-presence.cpp
	qfacebook/publish/qfacebook-publish-send-message-2.cpp
	qfacebook/publish/qfacebook-publish-send-message-response.cpp

	qfacebook/session/qfacebook-session-token.cpp
	qfacebook/session/qfacebook-session.cpp

	qfacebook/qfacebook-contact.cpp
	qfacebook/qfacebook-device-id.cpp
	qfacebook/qfacebook-json-reader.cpp
	qfacebook/qfacebook-login.cpp
	qfacebook/qfacebook-presence.cpp
	qfacebook/qfacebook-uid.cpp
	qfacebook/qfacebook-zlib.cpp

	qmqtt/qmqtt-connection.cpp
	qmqtt/qmqtt-message-receiver.cpp
	qmqtt/qmqtt-message-sender.cpp
	qmqtt/qmqtt-reader.cpp
	qmqtt/qmqtt-writer.cpp

	qthrift/qthrift-reader.cpp
	qthrift/qthrift-writer.cpp
	qthrift/qthrift.cpp

	services/facebook-chat-service.cpp
	services/facebook-contact-avatar-service.cpp
	services/facebook-roster-service.cpp

	widgets/facebook-add-account-widget.cpp
	widgets/facebook-edit-account-widget.cpp

	facebook-account-data.cpp
	facebook-module.cpp
	facebook-plugin-modules-factory.cpp
	facebook-plugin-object.cpp
	facebook-protocol.cpp
	facebook-protocol-factory.cpp
	facebook-services.cpp
)

find_package (ZLIB)

kadu_plugin (facebook_protocol
	PLUGIN_SOURCES ${SOURCES}
	PLUGIN_LIBRARIES ${ZLIB_LIBRARIES}
)
