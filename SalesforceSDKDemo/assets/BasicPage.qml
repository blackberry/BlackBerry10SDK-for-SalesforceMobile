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

import bb.cascades 1.2
import sf 1.0
import "helper_functions.js" as HelperJS

Page{
    ScrollView {
        horizontalAlignment: HorizontalAlignment.Fill
        
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            //Todo: fill me with QML
            Label {
                // Localized text with the dynamic translation and locale updates support
                text: qsTr("Salesforce SDK Demo") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.BigText
                horizontalAlignment: HorizontalAlignment.Center
            }
            Container {
                objectName: "optionsContainer"
                layout: StackLayout {
                
                }
                verticalAlignment: VerticalAlignment.Fill
                horizontalAlignment: HorizontalAlignment.Fill
                
                topMargin: 0.0
                topPadding: 20.0
                leftPadding: 20.0
                rightPadding: 20.0
                bottomPadding: 20.0
                Button {
                    id: cplusplusTestBtn
                    objectName: "cplusplusTestBtn"
                    text: "REST from C++"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                
                Button {
                    id: qmlTestBtn
                    objectName: "qmlTestBtn"
                    text: "REST from QML"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    onClicked: {
                        console.debug("[QML] Performing SOSL: FIND {Ac*} IN ALL FIELDS RETURNING Account (Id, Name), Contact Limit 5");
                        var sosl = SFRestAPI.generateSOSLSearchQuery("Ac*", "ALL FIELDS", {"Account":"Id, Name", "Contact":undefined}, 5);
                        var request = SFRestAPI.requestForSearch(sosl);
                        SFRestAPI.sendRestRequest(request, qmlTestBtn, qmlTestBtn.onTaskResultReady)
                    }
                    
                    function onTaskResultReady(result) {
                        console.debug("[QML] Result: "+result);
                        if (!result.hasError) {
                            var msg = HelperJS.payloadDescription(result.payload);
                            msg = "[QML] Code: %1, Message: %2\nPayload:\n%3".arg(result.code).arg(result.message).arg(msg);
                            console.debug(msg);
                            consoleLbl.text = msg;
                        } else {
                            var msg = "[QML] ErrorCode: %1 ErrorMessage: %2".arg(result.code).arg(result.message);
                            console.debug(msg);
                            consoleLbl.text = msg;
                        }
                    }
                    
                }
                
                Button {
                    id: oauthLoginBtn
                    objectName: "oauthLoginBtn"
                    text: "OAuth Login"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }
                Button {
                    id: oauthLogoutBtn
                    objectName: "oauthLogoutBtn"
                    text: "OAuth Logout"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }	
                
                Button {
                    id: remokeAccessBtn
                    objectName: "remokeAccessBtn"
                    text: "Revoke Access Token"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }

                Button {
                    id: revokeAllTokens
                    objectName: "revokeAllTokens"
                    text: "Revoke All Tokens"
                    layoutProperties: StackLayoutProperties {
                    
                    }
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                }                
                
                Label {
                    objectName:"oAuthLbl"
                    text: ""
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                }
                
                Label {
                    id: consoleLbl
                    objectName:"consoleLbl"
                    text: ""
                    multiline: true
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                }
            }
        }
        scrollViewProperties {
            scrollMode: ScrollMode.Vertical
        }
    }
}
