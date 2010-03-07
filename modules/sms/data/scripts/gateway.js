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

		this.result(this.gatewayFromNumber(match[1]));
	},

	result: function(gateway) {
		this.callbackObject.queryFinished(gateway);
	},

	gatewayFromNumber: function(number) {
		switch (number) {
			case "01": return "plus";
			case "02": return "era";
			case "03": return "oragne";
			case "06": return "play";
		}

		return "";
	}
};

function GatewayQuery() {
	return this;
}
