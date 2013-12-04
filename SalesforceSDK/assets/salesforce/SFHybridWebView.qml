import bb.cascades 1.2
Page {
	Container {
		// To enable scrolling in the WebView, it is put inside a ScrollView.
		ScrollView {
		    id: scrollView
		    
		    // We let the scroll view scroll in both x and y and enable zooming,
		    // max and min content zoom property is set in the WebViews onMinContentScaleChanged
		    // and onMaxContentScaleChanged signal handlers.
		    scrollViewProperties {
		        scrollMode: ScrollMode.Both
		        pinchToZoomEnabled: true
		    }
		    
		    WebView {
		        objectName: "webView"
                url: "local:///assets/www/index.html"
		        
		        // WebView settings, initial scaling and width used by the WebView when displaying its content.
		        settings.viewport: {
		            "width": "device-width",
		            "initial-scale": 1.0
		        }
		        
		        onLoadProgressChanged: {
		            // Update the ProgressBar while loading.
		            progressIndicator.value = loadProgress / 100.0
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
		        
		        onLoadingChanged: {
		            
		            if (loadRequest.status == WebLoadStatus.Started) {
		                // Show the ProgressBar when loading started.
		                progressIndicator.opacity = 1.0
		            } else if (loadRequest.status == WebLoadStatus.Succeeded) {
		                // Hide the ProgressBar when loading is complete.
		                progressIndicator.opacity = 0.0
		            } else if (loadRequest.status == WebLoadStatus.Failed) {
		                progressIndicator.opacity = 0.0
		            }
		        }		        
		    }
		}// ScrollView
		
		// A progress indicator that is used to show the loading status
		Container {
			bottomPadding: 25
			horizontalAlignment: HorizontalAlignment.Center
			verticalAlignment: VerticalAlignment.Bottom
			
			ProgressIndicator {
				id: progressIndicator
				opacity: 0
			}
		}
	}
}