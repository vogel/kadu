GatewayManager.prototype = {
	addItem: function(gateway) {
		this.items[gateway.id()] = gateway;
	},

	byId: function(gatewayId) {
		return this.items[gatewayId];
	},

	sendSms: function(gatewayId, recipient, sender, signature, content, callbackObject) {
		if (this.items[gatewayId]) {
			this.items[gatewayId].sendSms(recipient, sender, signature, content, callbackObject);
		} else {
			callbackObject.failure();
		}
	},
};

function GatewayManager() {
	this.items = new Object();
	return this;
}

gatewayManager = new GatewayManager();

GatewayQuery.prototype = {
	getGateway: function(phoneNumber, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.result("");
			return;
		}

		var gatewayCheckerUrl = "http://is.eranet.pl/updir/check.cgi?t=" + phoneNumber;

		this.reply = network.get(gatewayCheckerUrl);
		this.reply.finished.connect(this, this.replyFinished);
	},

	replyFinished: function() {
		if (!this.reply.ok()) {
			this.result("");
			return;
		}

		var content = this.reply.content();
		var pattern = new RegExp("260 ([0-9]{2})");
		var match = pattern.exec(content);

		if (null == match) {
			this.result("");
			return;
		}

		this.result(match[1]);
	},

	result: function(gateway) {
		this.callbackObject.queryFinished(gateway);
	},

	gatewayFromId: function(id) {
		var gateway = gatewayManager.byId(id);
		if (gateway) {
			return gateway.name();
		} else {
			return "";
		}
	}
};

function GatewayQuery() {
	return this;
}
