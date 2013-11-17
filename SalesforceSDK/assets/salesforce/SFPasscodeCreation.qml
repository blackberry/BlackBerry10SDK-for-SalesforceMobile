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
    id:createPassCodeSheet
    Page {
        Container {
            Label {
                text:"New Passcode"
            }
            TextField {
                id:newPasscode
                inputMode: TextFieldInputMode.Password
                input {                
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }
                onFocusedChanged: {
                    if (focused) {
                        newPasscode.editor.setSelection(0, newPasscode.text.length);
                    }
                }
            }            
            Label {
                text:"Confirm Passcode"
            }
            TextField {
                id: confirmedPasscode
                inputMode: TextFieldInputMode.Password
                input {                
                    submitKey: SubmitKey.Done;
                    onSubmitted: {
                        createPassCodeSheet.confirmPasscode();
                    }
                    flags: TextInputFlag.SpellCheckOff | TextInputFlag.PredictionOff
                }  
                onFocusedChanged: {
                    if (focused) {
                        confirmedPasscode.editor.setSelection(0, confirmedPasscode.text.length);
                    }
                }      
            }
            Label {
                id:infoLabel;
            }
        }
        titleBar: TitleBar {
            title: qsTr("Create Passcode")
            acceptAction: ActionItem {
                title: "Done"
                onTriggered: {
                    createPassCodeSheet.confirmPasscode();
                }
            }
        } 
    }//page
    peekEnabled: false;
    
    onOpened: {
        newPasscode.requestFocus();
    }
    function confirmPasscode() {
        if (newPasscode.text == confirmedPasscode.text){
            if (newPasscode.text.length >= SFSecurityLockout.requiredPasscodeLength()){                
                SFSecurityLockout.savePasscode(newPasscode.text);   
                newPasscode.text = "";
                confirmedPasscode.text = "";   
            }else{
                infoLabel.setText(qsTr("Your passcode must be at least " + SFSecurityLockout.requiredPasscodeLength() + " characters long"));
                newPasscode.requestFocus();
            }
        }else{
            infoLabel.setText(qsTr("Passcodes do not match"));
            newPasscode.requestFocus();   
        }
    }
}