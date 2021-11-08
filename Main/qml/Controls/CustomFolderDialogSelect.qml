import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2
import Qt.labs.platform 1.1


Item {
    property alias file_path: filespath_text_field.text
    property var txtInput: ""
    property var txtTxtFieldInput: ""

    RowLayout
    {
        id:filedialogBarId
        width: parent.width

        FolderDialog
        {
            id: file_dialog
        }


        Connections
        {
            target: file_dialog
            onAccepted:
            {
                filespath_text_field.text = file_dialog.folder
            }
        }

        Text
        {
            Layout.preferredWidth: 70
            Layout.fillHeight: true
            text: txtInput
            color: "#deb887"
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
            font.bold: true
        }

        TextField
        {
            id: filespath_text_field
            readOnly: true
            selectByMouse: true
            Layout.fillWidth: true
            placeholderText:txtTxtFieldInput
            background: Rectangle {
                anchors.fill: parent
                radius: 3
                border.color: "#333"
                border.width: 2
            }
        }

        CusotomButton1
        {
            Layout.preferredWidth: 70
            Layout.fillHeight: true
            txtBtn: "Browse"
            onEvent:
            {
                filespath_text_field.text = file_path
                var index = file_path.lastIndexOf('/')
                if (index !== -1)
                {
                    file_dialog.folder = file_path.substring(0, index)
                    console.log("file_dialog.folder  " + file_dialog.folder )

                }

                file_dialog.open();
            }
         }
    }
}
