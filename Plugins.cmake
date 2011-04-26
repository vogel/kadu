set (COMPILE_PLUGINS

#protocols
	# GaduGadu protocol suppot
	gadu_protocol
	# Jabber/XMPP protocol support
	jabber_protocol

# notifiers
	# Enables notifications about buddies presence and other in chat windows
	chat_notifier

# history
	# General history plugin
	history
	# Chat history storing, browsing and searching support using sqlite
	sql_history

# docking
	# Tray icon support (common part of all docking modules)
	docking
	# Always on top window docking plugin
	desktop_docking
	# Qt tray docking techniques module (works everywhere)
	qt4_docking

# sound
	# General sound plugin
	sound
	# ALSA sound support
	alsa_sound

# misc
	# Antistring
	antistring
	# Auto away module
	autoaway
	# Auto hide Kadu window
	auto_hide
	# Autoresponder plugin
	autoresponder
	# Autostatus
	autostatus
	# Cenzor
	cenzor
	# Configuration wizard
	config_wizard
	# Idle time counter
	idle
	# Shows image links and youtube links as images and videos in chat
#	imagelink
	# Sms gateway support
	sms
)
