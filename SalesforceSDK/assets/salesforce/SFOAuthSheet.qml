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