import GSettings 1.0
import QtQuick 2.12
import QtMultimedia 5.6
import SystemSettings 1.0
import Lomiri.Content 1.3
import Lomiri.Components 1.3
import Lomiri.Components.ListItems 1.3 as ListItem
import Lomiri.SystemSettings.Sound 1.0
import QMenuModel 1.0

import "utilities.js" as Utilities

ItemPage {
    property variant soundDisplayNames:
        Utilities.buildSoundValues(soundFileNames)
    property variant soundFileNames: refreshSoundFileNames()
    property bool showStopButton: false
    property int soundType // 0: ringtone, 1: message
    property string soundsDir
    property var activeTransfer

    onSoundFileNamesChanged: {
        soundDisplayNames = Utilities.buildSoundValues(soundFileNames)
        updateSelectedIndex()
    }

    id: soundsPage
    flickable: scrollWidget

    function refreshSoundFileNames() {
        var customDir = mountPoint + "/custom/usr/share/" + soundsDir;
        if (soundType === 0) {
            return backendInfo.listSounds([soundsDir, customDir, backendInfo.customRingtonePath])
        } else if (soundType === 1) {
            return backendInfo.listSounds([soundsDir, customDir, backendInfo.customMessageSoundPath])
        } else {
            return backendInfo.listSounds([soundsDir, customDir])
        }
    }

    LomiriSoundPanel {
        id: backendInfo
        onIncomingCallSoundChanged: {
            if (soundType == 0)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                                                    incomingCallSound)
        }
        onIncomingMessageSoundChanged: {
            if (soundType == 1)
                soundSelector.selectedIndex =
                        Utilities.indexSelectedFile(soundFileNames,
                                                    incomingMessageSound)
        }
    }

    GSettings {
        id: soundSettings
        schema.id: "com.lomiri.touch.sound"
    }

    QDBusActionGroup {
        id: soundActionGroup
        busType: DBus.SessionBus
        busName: "org.ayatana.indicator.sound"
        objectPath: "/org/ayatana/indicator/sound"

        Component.onCompleted: start()
    }

    Audio {
        id: soundEffect
        audioRole: MediaPlayer.NotificationRole
    }

    function setRingtone(path) {
        if (soundType == 0) {
            soundSettings.incomingCallSound = path
            backendInfo.incomingCallSound = path
        } else if (soundType == 1) {
            soundSettings.incomingMessageSound = path
            backendInfo.incomingMessageSound = path
        }
        soundFileNames = refreshSoundFileNames()
        previewTimer.start()
        soundEffect.source = path
        soundEffect.play()
    }

    function updateSelectedIndex() {
        if (soundType == 0)
            soundSelector.selectedIndex =
                    Utilities.indexSelectedFile(soundFileNames,
                        backendInfo.incomingCallSound)
        else if (soundType == 1)
            soundSelector.selectedIndex =
                    Utilities.indexSelectedFile(soundFileNames,
                        backendInfo.incomingMessageSound)
    }

    Flickable {
        id: scrollWidget
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: selectorColumn.childrenRect.height + stopItem.height
        boundsBehavior: (contentHeight > height) ?
                            Flickable.DragAndOvershootBounds :
                            Flickable.StopAtBounds
        /* Set the direction to workaround https://bugreports.qt-project.org/browse/QTBUG-31905
           otherwise the UI might end up in a situation where scrolling doesn't work */
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: selectorColumn
            anchors.left: parent.left
            anchors.right: parent.right

            ListItem.ItemSelector {
                id: soundSelector
                expanded: true
                model: soundDisplayNames
                selectedIndex: {
                    updateSelectedIndex()
                }
                onDelegateClicked: {
                    setRingtone(soundFileNames[index])
                }
            }

            ListItem.Standard {
                id: customRingtone
                text: (soundType === 1) ? i18n.tr("Custom message sound") : i18n.tr("Custom Ringtone")
                progression: true
                onClicked: pageStack.addPageToNextColumn(soundsPage, picker)
            }
        }
    }

    //Button to stop audio preview, ie. ringtone preview
    ListItem.SingleControl {
        id: stopItem
        anchors.bottom: parent.bottom
        enabled: soundEffect.playbackState == Audio.PlayingState
        visible: enabled
        control: AbstractButton {
            id: stopButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            focus: false
            width: height
            height: units.gu(4)
            visible: stopItem.enabled

            onClicked: soundEffect.stop()

            Rectangle {
                anchors.fill: parent
                radius: width * 0.5
                border.color: theme.palette.normal.raisedText
                border.width: 1
            }

            Rectangle {
                width: parent.height * 0.4
                height: width
                smooth: true
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
                color: theme.palette.normal.raisedText
            }
        }
        Rectangle {
            anchors.fill: parent
            z: parent.z - 1
            visible: stopItem.visible
            color: Theme.palette.normal.background
        }
    }

    Timer {
        id: previewTimer
        onTriggered: soundEffect.stop()
        interval: 30000
    }

    Connections {
        id: contentHubConnection
        property var ringtoneCallback
        target: activeTransfer ? activeTransfer : null
        onStateChanged: {
            if (activeTransfer.state === ContentTransfer.Charged) {
                if (activeTransfer.items.length > 0) {
                    var customSoundPath = (soundType === 0) ? backendInfo.customRingtonePath : backendInfo.customMessageSoundPath
                    var item = activeTransfer.items[0];
                    var toneUri;
                    if (item.move(customSoundPath)) {
                        toneUri = item.url;
                    } else {
                        toneUri = customSoundPath + "/" + item.url.toString().split("/").splice(-1,1);
                    }
                    ringtoneCallback(toneUri);
                }
            }
        }
    }

    Page {
        id: picker
        visible: false
        header: PageHeader {
            title: i18n.tr("Choose from")
        }

        ContentPeerPicker {
            id: peerPicker
            visible: parent.visible
            anchors.top: header.bottom
            anchors.topMargin: units.gu(1)
            handler: ContentHandler.Source
            contentType: ContentType.Music
            showTitle: false

            onPeerSelected: {
                pageStack.removePages(soundsPage);
                // requests an active transfer from peer
                function startContentTransfer(callback) {
                    if (callback)
                        contentHubConnection.ringtoneCallback = callback
                    var transfer = peer.request();
                    if (transfer !== null) {
                        soundsPage.activeTransfer = transfer;
                    }
                }
                peer.selectionType = ContentTransfer.Single;
                startContentTransfer(function(uri) {
                    setRingtone(uri.toString().replace("file:///", "/"));
                });
            }

            onCancelPressed: pageStack.removePages(soundsPage);
        }
    }

    ContentTransferHint {
        anchors.fill: parent
        activeTransfer: soundsPage.activeTransfer
    }

    Component.onDestruction: {
        if (soundEffect.playbackState === Audio.PlayingState) {
            soundEffect.stop()
        }
    }
}
