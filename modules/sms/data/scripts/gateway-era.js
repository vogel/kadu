GatewayEra.prototype = {
	name: function() {
		return "era";
	},

	id: function() {
		return "02";
	}
};

function GatewayEra() {
	return this;
}

gatewayManager.addItem(new GatewayEra());
