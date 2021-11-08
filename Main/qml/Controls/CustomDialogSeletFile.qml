import QtQuick 2.12
import QtQuick.Controls.Styles 1.4

import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.2

Item {
    property alias file_path: filespath_text_field.text
    property var txtInput: ""
    property var txtTxtFieldInput: ""

    RowLayout
    {
        id:filedialogBarId
        width: parent.width

        FileDialog
        {
            id: file_dialog
            selectMultiple: false
        }


        Connections
        {
            target: file_dialog
            onAccepted:
            {
                if (file_dialog.fileUrls.length > 0)
                {
                    var splitStringArr = file_dialog.fileUrls[0].split("/")
                    filespath_text_field.text = splitStringArr[splitStringArr.length -1]
                }
                else
                    filespath_text_field.text = ""
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
