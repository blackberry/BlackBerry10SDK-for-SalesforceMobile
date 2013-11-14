import bb.cascades 1.0

Sheet {
    id:oauthSheet
    Page {
        titleBar: TitleBar {
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                Container {
                    layout: StackLayout { orientation: LayoutOrientation.LeftToRight }
                    leftPadding: 10
                    rightPadding: 10
                    Button {
                        text: "Cancel"
                        id: cancelAction
                        objectName: "cancelAction"
                        verticalAlignment: VerticalAlignment.Center
                        onClicked: {
                            oauthSheet.cancel();
                        }
                    }                    
                    Label {
                        text: "Login"
                        verticalAlignment: VerticalAlignment.Center
                        layoutProperties: StackLayoutProperties { spaceQuota: 1 }
                        textStyle {
                            color: Color.White
                            fontSize: FontSize.Medium
                            fontWeight: FontWeight.Bold
                        }
                    }
                    ActivityIndicator {
                        id: indicator
                        objectName: "indicator"
                        preferredWidth: 100
                        verticalAlignment: verticalAlignment.Center
                    }
                }
            }
            scrollBehavior: TitleBarScrollBehavior.Default
        }
        Container {
            verticalAlignment: VerticalAlignment.Fill
            horizontalAlignment: HorizontalAlignment.Fill
            implicitLayoutAnimationsEnabled: false
            ScrollView {
                id: oauthScrollView
                objectName: "oauthScrollView"
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                implicitLayoutAnimationsEnabled: false
                layoutProperties: StackLayoutProperties {
                    spaceQuota: 1.0
                }
                scrollViewProperties {
                    scrollMode: ScrollMode.Vertical
                }
                
                attachedObjects: [
                    LayoutUpdateHandler {
                        id: layoutHandler
                    }
                ]
                
                onContentChanged: {
                    if (!(content instanceof Object)) {
                        return;
                    }
                    content.minWidth = layoutHandler.layoutFrame.width;
                    content.minHeight = layoutHandler.layoutFrame.height;
                }
            }
        }
    }
    peekEnabled: false

    function cancel() {
        if (oauthSheet.opened){
            oauthScrollView.content=null;
            oauthSheet.close();
        }
        SFAuthenticationManager.cancelAuthentication();
    }
}