GatewayPlay.prototype = {
	name: function() {
		return "Play";
	},

	id: function() {
		return "06";
	}
};

function GatewayPlay() {
	return this;
}

gatewayManager.addItem(new GatewayPlay());
