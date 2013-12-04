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

import bb.cascades 1.2
Container {
	// To enable scrolling in the WebView, it is put inside a ScrollView.
	ScrollView {
	    id: scrollView
	    	    
	    scrollViewProperties {
	        scrollMode: ScrollMode.Both
	        pinchToZoomEnabled: false
	    }
	    
	    WebView {
	        objectName: "webView"
            url: "local:///assets/www/index.html"
	        
	        // WebView settings, initial scaling and width used by the WebView when displaying its content.
	        settings.viewport: {
	            "width": "device-width",
	            "initial-scale": 1.0
	        }
	        	        
	        onMinContentScaleChanged: {
	            // Update the scroll view properties to match the content scale
	            // given by the WebView.
	            scrollView.scrollViewProperties.minContentScale = minContentScale;
	            
	            // Let's show the entire page to start with.
	            scrollView.zoomToPoint(0,0, minContentScale,ScrollAnimation.None)
	        }
	        
	        onMaxContentScaleChanged: {
	            // Update the scroll view properties to match the content scale 
	            // given by the WebView.
	            scrollView.scrollViewProperties.maxContentScale = maxContentScale;
	        }	        
	    }
	}// ScrollView	
}