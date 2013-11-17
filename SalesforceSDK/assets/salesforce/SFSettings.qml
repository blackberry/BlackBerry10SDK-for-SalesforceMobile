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

Page {
    Container {
        RadioGroup {
            id: connectionsGroup
            objectName: "connectionsGroup"

            Option {
                id: productionHost
                value: "login.salesforce.com"
                text: "Production"    
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            
            Option {
                id: sandboxHost
                value: "test.salesforce.com"
                text: "Sandbox"                
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            
            Option {
                id: customHost
                value: "CUSTOM"
                text: "Custom Host"                
                selected: SFAccountManager.getLoginHostFromAppSettings() ==  value? true : false                            
            }
            onSelectedOptionChanged: {
                SFAccountManager.setLoginHostToAppSettings(selectedOption.value);
            }
        } // RadioGroup
        
        TextField {
            id:customHostValue
            text: SFAccountManager.getCustomHostFromAppSettings();
            input {                
                submitKey: SubmitKey.Done;
                onSubmitted: {
                    SFAccountManager.setCustomHostToAppSettings(customHostValue.text);
                }
                flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
            }        
            onTextChanged: {
                console.debug("on touch exit");
                SFAccountManager.setCustomHostToAppSettings(customHostValue.text);                
            }
        }
    }
}