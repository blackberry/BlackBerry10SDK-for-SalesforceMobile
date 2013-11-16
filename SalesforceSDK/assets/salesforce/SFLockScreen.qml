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

Sheet {
    id:lockScreenSheet
    Page {
        Container {
            TextField {
                id:passcodeField
                inputMode: TextFieldInputMode.Password
                input {                
                    submitKey: SubmitKey.Done;
                    onSubmitted: {
                        lockScreenSheet.validatePasscode(passcodeField.text)
                    }
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }  
            }
            Button {
                id:forgotPasscodeBtn
                text:qsTr("Forgot Passcode");
                onClicked: {
                    SFSecurityLockout.reset();
                    SFAuthenticationManager.logout();
                }
            }
            Label {
                id:infoLabel
            }
        }
        titleBar: TitleBar {
            title: qsTr("Verify Passcode")
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    lockScreenSheet.validatePasscode(passcodeField.text)
                }
            }
        }
    }
    peekEnabled:false;
    
    onOpened: {
        passcodeField.requestFocus();    
    }
    
    function validatePasscode(passcode){
        if (!SFSecurityLockout.validatePasscode(passcode)){
         	infoLabel.text = qsTr("The passcode you entered was invalid");
            passcodeField.requestFocus();
            passcodeField.editor.setSelection(0, passcodeField.text.length); 
        }else{
            passcodeField.text = "";
        }
    }
}