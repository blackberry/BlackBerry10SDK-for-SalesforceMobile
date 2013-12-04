/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

var sf = {
	// generating callbackIds - using the same technique found in cordova.js
    // Randomize the starting callbackId to avoid collisions after refreshing or navigating.
    // This way, it's very unlikely that any new callback would get the same callbackId as an old callback.
	callbackId: Math.floor(Math.random() * 2000000000),
	//map of callbackId to callbacks
	callbacks: {},
	//matches with the enum in SFPluginResult.h
    callbackStatus: {
        NO_RESULT: 0,
        OK: 1,
        CLASS_NOT_FOUND_EXCEPTION: 2,
        ILLEGAL_ACCESS_EXCEPTION: 3,
        INSTANTIATION_EXCEPTION: 4,
        MALFORMED_URL_EXCEPTION: 5,
        IO_EXCEPTION: 6,
        INVALID_ACTION: 7,
        JSON_EXCEPTION: 8,
        ERROR: 9
    },
    /*
     * Used by SFPlugin to trigger callback when plugin finishes executing the native code
     */
	executeCallback:function(callbackId, status, message, keepCallback){
		var callback = sf.callbacks[callbackId];
        if (callback) {
        	if (status == sf.callbackStatus.OK){
                callback.success && callback.success(message);        		
        	}else{
                callback.fail && callback.fail(message);
        	}
            // Clear callback if not expecting any more results
            if (!keepCallback) {
                delete sf.callbacks[callbackId];
            }
        }	
	},
	/*
	 * successCallBack is the call back function if the plugin executed successfully
	 * failCallBack is if the plugin couldn't be invoked properly
	 * className is the corresponding fully qualified class name on native side, i.e. sf::SFAuthPlugin
	 * methodName is the method on the plugin to be called
	 * args is a json object where each attribute represents an argument to be passed into the native code. 
	 */
	exec:function(successCallBack, failCallBack, className, methodName, args){
		callbackId = className + "_" + methodName + "_" + sf.callbackId++;
		sf.callbacks[callbackId] = {success:successCallBack, fail:failCallBack};		
		//serialize this information and post it to native
		var jsonData = {"callbackId":callbackId,"className":className,"methodName":methodName,"arguments":args};
		console.log(JSON.stringify(jsonData));
		navigator.cascades.postMessage(JSON.stringify(jsonData));
	}
};

/*
 * matches the enum of same name defined in SFGlobal.h
 */
var HTTPMethodType = {
	HTTPGet:0, //!< HTTP GET
	HTTPHead:1, //!< HTTP HEAD
	HTTPDelete:2, //!< HTTP DELETE
	HTTPPost:3, //!< HTTP POST
	HTTPPut:4, //!< HTTP PUT
	HTTPPatch:5 //!< HTTP PATCH
};

/*
 * matches the enum of the same name defined in SFRestRequest.h
 */
var HTTPContentType = {
	HTTPContentTypeUrlEncoded:0, /*!< parameters are encoded into the URL as key-value pair. e.g. "?firstname=John&lastname=Smith" */
	HTTPContentTypeJSON:1, /*!< parameters are encoded into request body using JSON format. */
	HTTPContentTypeMultiPart:2 /*!< use HTTP "multipart/form-data" encoding. @warning currently not implemented. */
};