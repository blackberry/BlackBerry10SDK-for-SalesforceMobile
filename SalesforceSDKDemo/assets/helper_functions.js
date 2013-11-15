
function payloadDescription(payload) {
	var discription = payloadDescriptionRecursive('', payload, 0);
	return discription;
}

function payloadDescriptionRecursive(key, payload, depth) {
	var intent = "";
	var oneIntent = "      ";
	for(var i = 0; i < depth; i++) {
		intent += oneIntent;
	}
	var description = intent;
	if (key.length != 0) {
		description += key + ': ';
	}
	if (payload instanceof Array) {
		description += '[\n';
		for(var k in payload) {
			description += payloadDescriptionRecursive('', payload[k], depth+1) + ',\n';
		}
		description += intent + ']';
	} else if (payload instanceof Object) {
		description += '{\n';
		for(var k in payload) {
			description += payloadDescriptionRecursive(k, payload[k], depth+1) + ',\n';
		}
		description += intent + '}';
	} else {
		description += payload;
	}
	return description;
}