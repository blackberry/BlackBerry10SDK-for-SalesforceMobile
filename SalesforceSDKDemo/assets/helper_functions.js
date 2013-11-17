/*
* Copyright 2013 BlackBerry Limited.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

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