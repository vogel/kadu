GatewayQuery.prototype = {
	getGateway: function(phoneNumber, callbackObject) {
		this.callbackObject = callbackObject;

		if (!network) {
			this.callbackObject.query2Finished("");
			return;
		}

		var internationalPhoneNumber = "48" + phoneNumber;
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
		var pattern = new RegExp("260 ([0-9]{3})");
		this.callbackObject.query2Finished(pattern);
		var match = patter.exec(content);
		this.callbackObject.query2Finished(match);

		if (null == match) {
			this.callbackObject.query2Finished("");
			return;
		}

		this.callbackObject.query2Finished(match[1]);
	}
};

function GatewayQuery() {
	return this;
}
