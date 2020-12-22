// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    console.log("Ready");

    //Update the File log table
    updateFileLogTable();

    //Update the dropdown in card view panel
    updateDropDownSelect();

    //Check if there are cards for card view
    updateCardView();

    //Creating a new VCard
    $('#createVCardForm').on("submit", function(event) {
        
        event.preventDefault();

        let fileName = $('#inputFileName').val();
        let FN = $('#inputFN').val();
        
        fileName.toString();
        FN.toString();

        let package = {};
        package.fileName = fileName;
        package.FN = FN;

        /* Check file if filename already exists */
        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/listFiles',
            success: function(cardData) {
                let exists = false;

                for(let i = 0; i < cardData.list.length; i++) {
                    if(cardData.list[i].fileName == fileName) {
                        exists = true;
                    }
                }

                /* If file doesnt exist, create file*/
                if(exists == false) {
                    $.ajax({
                        type: 'get',
                        datatype: 'json',
                        url: '/createFile',
                        data: package,
                        success: function(data) {
                            if(data.valid == true) {
                                console.log("SUCCESS createVCard");
                            }
                            else if(data.valid == false) {
                                alert("The inputs entered to create a card were invalid, please try again. Remember file extension must only be '.vcf' or '.vcard'. FN has only a cardinality of 1.");
                                console.log("Could not createVCard")
                            }
                        },
                        fail: function(data) {
                            console.log("Failed createVCard");
                            alert("The inputs entered to create a card were invalid, please try again. Remember file extension must only be '.vcf' or '.vcard'. FN has only a cardinality of 1.");
                        }
                    })
                } else {
                    alert("File with name '" + fileName + "' already exists, please choose new name.");
                    console.log("File " + fileName + " already exists, choose a new name");
                }
            },
            fail: function(cardData) {
                console.log("Unable to retrieve list of files for checking fileNames");
            }
        });
    });

    /* When clicking on an option on the dropdown list 
       Display the data from the file selected */
    $('#dropdownList').change(function() {
        let file = $('#dropdownList').val();
        let index = 0;

        file.toString();

        let sendData = {};
        sendData.fileName = file;

        changeCardViewTable(sendData);
    });

    /* When clicking the show param button we want to fill
       in the table on the modal */
    $('#showParam').on("click", function() {

        let fileName = $('#dropdownList').val();
        
        sendData = {};
        sendData.fileName = fileName;

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/getFile',
            data: sendData,
            success: function(data) {
                /* Make sure modal form is clear */
                $('#showParamTable').find('#showParamBody').empty();

                /* Fill the modal form with the data */
                let paramStr;
                if(data.card.fn.parameters.length == 0) {
                    paramStr = "No Parameters"
                } else {
                    paramStr = createParametersString(data.card.fn.parameters);
                }

                /* Append the FN property*/
                $('#showParamBody').append("<tr>");
                $('#showParamBody').append("<td>FN</td>");
                $('#showParamBody').append("<td>" + paramStr + "</td>");
                $('#showParamBody').append("</tr>");

                /*Append the optional properties */
                for(let i = 0; i < data.card.optionalProperties.length; i++) {

                    /* If there are no parameters */
                    if(data.card.optionalProperties[i].parameters == 0) {
                        paramStr = "No Parameters"
                    } else {
                        paramStr = createParametersString(data.card.optionalProperties[i].parameters);
                    }

                    $('#showParamBody').append("<tr>");
                    $('#showParamBody').append("<td>" + data.card.optionalProperties[i].name + "</td>");
                    $('#showParamBody').append("<td>" + paramStr + "</td>");
                    $('#showParamBody').append("</tr>");
                }
                console.log(data.card.fn.parameters);
            }

        })
    });

    /* When clicking the edit values button populate the modal */
    $('#editValues').on("click", function() {
        let file = $('#dropdownList').val();
        updateEditValDropDown(file);

        /* Make first element in dropdown displays in the val form */
        updateEditFormValues(file);
    });

    /* When changing the dropdown list in edit values display the
       appropriate property to edit */
    $('#dropdownValues').change(function() {
        let file = $('#dropdownList').val();
        updateEditFormValues(file);

    });

    /* When clicking submit button for edit values, add it to card */
    $('#submitEditVal').on("click", function(event) {
        event.preventDefault();

        let file = $('#dropdownList').val();
        let index = $('#dropdownValues').val();
        let value = $('#propertyVal').val();

        sendData = {};
        sendData.fileName = file;
        sendData.index = index;
        sendData.value = value;

        console.log(sendData.fileName);
        console.log(sendData.index);
        console.log(sendData.value);

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/editValue',
            data: sendData,
            success: function(data) {
                if(data.isUpdated == true) {
                    console.log("Successfully updated the file with editd value");
                }
                else if (data.isUpdated == false) {
                    console.log("Unsuccessfully updated the file with edited values");
                    alert("Could not successfully update the file. Please try again. Remember to not use commas as it can only be a single value");
                }
            },
            fail: function(data) {
                console.log("Failed ajax call for /editValue");
            }
        });
    });

    /* When clicking submit for Add property it adds it to the card */
    $('#submitAddProp').on("click", function(event) {
        event.preventDefault();

        let file = $('#dropdownList').val();
        let propName = $('#propName').val();
        let propVal = $('#propValues').val();

        let sendData = {};
        sendData.fileName = file;
        sendData.propName = propName;
        sendData.propVal = propVal;

        console.log("HI");

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/addProp',
            data: sendData,
            success: function(data) {
                if(data.addedProperty == true) {
                    console.log("Added Property");
                } else {
                    alert("Invalid Property, please try again");
                    console.log("Did not add property");
                }
            },
            fail: function(data) {
                console.log("Failed connecting to /addProp API");
            }
        });
    });

});

/* Function to retrieve data to update file log table */
function updateFileLogTable() {
    $.ajax( {
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/listFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /* Update the file log table with the list */
            console.log(data);
            console.log("Update Log Panel");

            if(data.retFile == true) {
                /* If no files on Server display no files */
                if(data.list.length == 0) {
                    $('#fileLogBody').append("<tr>");
                    $('#fileLogBody').append("<td colspan = \"3\">NO FILES FOUND ON SERVER</td>");
                    $('#fileLogBody').append("</tr>");
                } else {
                    for(let i = 0; i < data.list.length; i++) {
                        $('#fileLogBody').append("<tr>");
                        $('#fileLogBody').append("<td><a href=\"" + data.list[i].fileName + "\" download>" + data.list[i].fileName + "</a></td>");
                        $('#fileLogBody').append("<td>" + data.list[i].fn.values[0] + "</td>");
                        $('#fileLogBody').append("<td>" + data.list[i].numProp + "</td>");
                        $('#fileLogBody').append("</tr>");
                    }
                }
            } else {
                console.log("Could not retrieve all files");
                alert("File was not able to be retrieved");
            }
        },
        fail: function(data) {
            console.log("Failed retrieving object from updateFileLogTable()");
        }
    });
}

/* Updates the dropdown select with files */
function updateDropDownSelect() {
    $.ajax( {
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/listFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /* Update the file log table with the list */
            console.log(data);
            console.log("Update Drop Down select");
            
            /* If nothing in uploads */
            if(data.list.length == 0) {
                $('#dropdownList').append("<option class=\"dropdown-item\" value=\"\">No Cards Available</option>");
            }

            for(let i = 0; i < data.list.length; i++) {
                if(data.retFile == true) {                
                    $('#dropdownList').append("<option class=\"dropdown-item\" value=\""+ data.list[i].fileName +"\">"+ data.list[i].fileName+"</option>");

                    if(i == 0) {
                        fileName = data.list[0].fileName;
                        sendData = {};
                        sendData.fileName = fileName;
                        changeCardViewTable(sendData);
                    }
                } else {
                    console.log("Could not retrieve all files");
                }
            }
        },
        fail: function(data) {
            console.log("Failed retrieving JSON in updateDropDownSelect()");
        }
    });
}

/* Updates the cardView panel if theres no files on server */
function updateCardView() {
    $.ajax( {
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/listFiles',   //The server endpoint we are connecting to
        success: function (data) {
            /* Update the file log table with the list */
            console.log(data);
            console.log("Update Drop Down select");

            if(data.list.length == 0) {              
                $('#viewCardBody').append("<tr>");
                $('#viewCardBody').append("<td colspan = \"3\">NO FILES FOUND ON SERVER</td>");
                $('#viewCardBody').append("<tr>");
            }
        },
        fail: function(data) {
            console.log("Failed retrieving JSON in updateCardView()");
        }
    });
}

/* Update the card View Panel Table */
function changeCardViewTable(sendData) {
    /* If theres any content in the table then we want to remove it */
    $('#cardViewTable').find('#viewCardBody').empty();

    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/getFile',   //The server endpoint we are connecting to
        data: sendData,
        success: function (data) {
            console.log("Updating view card to display the chosen file");

            let prop = 1;

            /* INSERT THE VALID PROPERTIES 
               FN PROPERTY */
            $('#viewCardBody').append("<tr>");
            $('#viewCardBody').append("<td>1</td><td>FN</td>");
            $('#viewCardBody').append("<td>" + data.card.fn.values[0] + "</td>");
            $('#viewCardBody').append("<td>" + data.card.fn.parameters.length + "</td>");
            $('#viewCardBody').append("</tr>");
            prop++;

            /* Insert birthday*/
            if(data.card.birthday) {
                $('#viewCardBody').append("<tr>");
                $('#viewCardBody').append("<td>2</td><td>BDAY</td>");
                if(data.card.birthday.isText == true) {
                    $('#viewCardBody').append("<td>" + data.card.birthday.text + "</td>");
                } 
                else {
                    let d = "Date: " + data.card.birthday.date + " ";
                    let t = "Time: " + data.card.birthday.time;
                    let str = d + t;
                    $('#viewCardBody').append("<td>" + str + "</td>");
                }
                $('#viewCardBody').append("<td>0</td>");
                $('#viewCardBody').append("</tr>");
                prop++;
            }

            /* Insert anniversary */
            if(data.card.anniversary) {
                $('#viewCardBody').append("<tr>");
                $('#viewCardBody').append("<td>2</td><td>ANNIVERSARY</td>");
                if(data.card.anniversary.isText == true) {
                    $('#viewCardBody').append("<td>" + data.card.anniversary.text + "</td>");
                } 
                else {
                    let a = "Date: " + data.card.anniversary.date + " ";
                    let b = "Time: " + data.card.anniversary.time;
                    let t = a + b;
                    $('#viewCardBody').append("<td>" + t + "</td>");
                }
                $('#viewCardBody').append("<td>0</td>");
                $('#viewCardBody').append("</tr>");
                prop++;
            }

            /* Optional Properties */
            for(let i = 0, j = prop; i < data.card.optionalProperties.length; i++,j++) {
                let str = createValueString(data.card.optionalProperties[i].values);
                $('#viewCardBody').append("<tr>");
                $('#viewCardBody').append("<td>" + j + "</td>");
                $('#viewCardBody').append("<td>" + data.card.optionalProperties[i].name + "</td>");
                $('#viewCardBody').append("<td>" + str + "</td>");
                $('#viewCardBody').append("<td>" + data.card.optionalProperties[i].parameters.length + "</td>");
                $('#viewCardBody').append("</tr>");
            }
    
        },
        fail: function() {
            console.log("Unsuccessfully retrieved data in changeCardViewTable(sendData)");
        }
    });
}

/* Creates a string for the property values */
function createValueString(data) {
    let index;
    
    /* Make sure starting index isnt empty otherwise there will be a leading comma */
    for(let i = 0; i < data.length; i++) {
        if(data[i] != "") {
            index = i;
            break;
        }
    }

    let valStr = data[index];

    for(let i = index + 1; i < data.length; i++) {
        if(data[i] != "") {
            valStr = valStr + ", " + data[i];
        }
    }

    return valStr;
}

/* Creates a string for the parameter values from array */
function createParametersString(data) {
    /* Declaring variables */
    let retStr;
    let buff;
    let str;

    /* Get the first first parameter name and value into the return string */
    retStr = data[0].name + "=" + data[0].value

    /* Loop through array and store the name and variables in the return
       string if there are more than 1 parameter. */
    for(let i = 1; i < data.length; i++) {
        retStr = retStr + ", " + data[i].name + "=" + data[i].value;
    }

    str = retStr;

    return str;
}

/* Creates the edit values dropdown */
function updateEditValDropDown(file) {

    sendData = {};
    sendData.fileName = file;

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getFile',
        data: sendData,
        success: function(data) {
            
            /* Update the file log table with the list */
            console.log(data);
            console.log("Update Drop Down select for edit values");

            /*Empty dropdown*/
            $('#dropdownValues').empty();

            /* Append the FN value */
            $('#dropdownValues').append("<option class=\"dropdown-item\" value=\"-1\">FN</option>");

            /* Append the optional properties */
            // for(let i = 0; i < data.card.optionalProperties.length; i++) {
            //     $('#dropdownValues').append("<option class=\"dropdown-item\" value=\"" + data.card.optionalProperties[i].name + "\">" + data.card.optionalProperties[i].name + "</option>");
            // }

            /* Append the optional properties that only have 1 */
            for(let i = 0; i < data.card.optionalProperties.length; i++) {
                if(data.card.optionalProperties[i].values.length < 2) {
                    // $('#dropdownValues').append("<option class=\"dropdown-item\" value=\"" + data.card.optionalProperties[i].name + "\">" + data.card.optionalProperties[i].name + "</option>");
                    $('#dropdownValues').append("<option class=\"dropdown-item\" value=\"" + i + "\">" + data.card.optionalProperties[i].name + "</option>");
                }
            }
        },
        fail: function(data) {
            console.log("Failed to retrieve file from updateEditValDropDown()");
        }
    });
}

/* Update the edit value form to have the proper property values */
function updateEditFormValues(file) {
    sendData = {};
    sendData.fileName = file;

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getFile',
        data: sendData,
        success: function(data) {
            
            /* Update the file log table with the list */
            console.log(data);
            console.log("Update form for edit values");

            /*Empty dropdown*/
            $('#editValInput').empty();
            
            /* Get the property */
            let prop = $('#dropdownValues').val();
            let index;
            let valStr

            /* Retrieve the values string */
            // if(prop == 'FN') {
            //     valStr = createValueString(data.card.fn.values);
            // } 
            if(prop == -1) {
                valStr = createValueString(data.card.fn.values);
            }
            else {
                /* If property is an optional property */
                // for(let i = 0; i < data.card.optionalProperties.length; i++) {
                //     if(data.card.optionalProperties[i].name == prop) {
                //         index = i;
                //     }
                // }
                index = $('#dropdownValues').val();
                valStr =  createValueString(data.card.optionalProperties[index].values);
            }
            
            /* Insert the values into form */
            $('#editValInput').append("<input type=\"text\" class=\"form-control\" id=\"propertyVal\" value=\"" + valStr + "\" placeholder=\"Property Values\" required>");
        },
        fail: function(data) {
            console.log("Failed to retrieve file from updateEditFromValues()");
        }
    });
}