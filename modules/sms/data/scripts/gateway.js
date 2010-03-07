GatewayQuery.prototype = {
	getGateway: function(phoneNumber, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.callbackObject.query2Finished("");
			return;
		}

		var gatewayCheckerUrl = "http://is.eranet.pl/updir/check.cgi?t=" + phoneNumber;

		this.reply = network.get(gatewayCheckerUrl);
		this.reply.finished.connect(this, this.replyFinished);
	},

	replyFinished: function() {
		this.callbackObject.query2Finished("reply finished");
		if (!this.reply.ok()) {
			this.callbackObject.query2Finished("");
			return;
		}

		var content = this.reply.content();
		var pattern = new RegExp("260 ([0-9]{2})");
		var match = pattern.exec(content);

		if (null == match) {
			this.callbackObject.query2Finished("");
			return;
		}

		this.callbackObject.query2Finished(this.gatewayFromNumber(match[1]));
	},

	gatewayFromNumber: function(number) {
		switch (number) {
			case "01": return "plus";
			case "02": return "era";
			case "03": return "oragne";
			case "04": return "play";
		}

		return "";
	}
};

function GatewayQuery() {
	return this;
}
