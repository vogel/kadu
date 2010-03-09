GatewayPlay.prototype = {
	name: function() {
		return "play";
	},

	id: function() {
		return "06";
	}
};

function GatewayPlay() {
	return this;
}

gatewayManager.addItem(new GatewayPlay());
