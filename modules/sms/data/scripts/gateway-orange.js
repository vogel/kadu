GatewayOrange.prototype = {
	name: function() {
		return "orange";
	},

	id: function() {
		return "03";
	},

	sendSms: function(receipient, sender, signature, content, callbackObject) {
		var sender = new GatewaySmsSender();
		sender.sendSms(receipient, sender, signature, content, callbackObject);
	},
};

function GatewayOrange() {
	return this;
}

gatewayManager.addItem(new GatewayOrange());

GatewaySmsSender.prototype = {

	failure: function() {
		this.callbackObject.failure();
	},

	result: function(result) {
		this.callbackObject.result(result);
	},

	sendSms: function(receipient, sender, signature, content, callbackObject) {
		this.receipient = receipient;
		this.sender = sender;
		this.signature = signature;
		this.contetn = content;
		this.callbackObject = callbackObject;

		if (!network) {
			this.failure();
			return;
		}

		var formUrl = "http://sms.orange.pl/";

		this.reply = network.get(formUrl);
		this.reply.finished.connect(this, this.formReceived);
	},

	formReceived: function() {
		if (!this.reply.ok()) {
			this.failure();
			return;
		}

		var content = this.reply.content();
		var tokenPattern = new RegExp("rotate_token\\.aspx\\?token=([^\"]+)");
		var match = tokenPattern.exec(content);

		if (null == match) {
			this.failure();
			return;
		}

		this.result(match[0]);
	}
};

function GatewaySmsSender() {
	return this;
}
