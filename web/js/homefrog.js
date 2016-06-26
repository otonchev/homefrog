var urlRoot = 'http://ogy.noip.me:5678';
var urlHome = urlRoot + '/home';
var urlWeb = urlRoot + '/web';
var historySuffix = '?history';

var getJSON = function(url, requestType, successHandler, errorHandler) {
        var xhr = typeof XMLHttpRequest != 'undefined'
                ? new XMLHttpRequest()
                : new ActiveXObject('Microsoft.XMLHTTP');
        xhr.open(requestType, url, true);
        xhr.onreadystatechange = function() {
                var status;
                var data;
                if (xhr.readyState == 4) { // `DONE`
                        status = xhr.status;
                        if (status == 200) {
                                data = JSON.parse(xhr.responseText);
                                successHandler && successHandler(data);
                        } else {
                                errorHandler && errorHandler(status);
                        }
                }
        };
        xhr.send();
};

var generateHistory = function(historyUrl) {
        getJSON(historyUrl, 'get', function(jsonHistoryObj) {
                getJSON(urlWeb, 'get', function(jsonWebObj) {

                        var outputString = "";

                        for (var path in jsonHistoryObj) {
                                for (i = 0; i < jsonHistoryObj[path].length; i++) {
                                        var singleRecord = jsonHistoryObj[path][i];
                                        var pluginName = singleRecord.name;
                                        var timestamp = singleRecord["rest.timestamp"];

                                        var deviceType = jsonWebObj[pluginName]["device-type"];
                                        var displayOptions = jsonWebObj[pluginName]["display-options"];

                                        if (deviceType == "scene") {
                                                outputString = outputString + "activated";
                                        } else {
                                                for (j = 0; j < displayOptions.length; j++) {
                                                        var displayOption = displayOptions[j].option;
                                                        outputString = outputString + " " +singleRecord[displayOption];
                                                }
                                        }

                                        outputString = outputString + ", " + timestamp + "\n";
                                }
                        }

                        alert(outputString);
                }, function(status) {
                        alert('Something went wrong.');
                });
        }, function(status) {
                alert('Something went wrong.');
        });
}

var simpleSwitch = function(path, buttonName) {
        getJSON(urlRoot + path, 'post', function(jsonWebObj) {
                currentStatus = document.getElementById(buttonName).value;
                document.getElementById(buttonName).value = ((currentStatus == "on") ? "off" : "on");
        }, function(status) {
                alert('Something went wrong.');
        });
}

var displaySensors = function(jsonHomeObj, jsonWebObj) {
        var pathArray = new Array();

        //create Associative array where keys=paths and values are array of objects
        //describing sensors belonging to path

        for (var sensor in jsonHomeObj) {
                if (jsonHomeObj.hasOwnProperty(sensor)) {
                        var n = sensor.lastIndexOf("/");
                        var pathToSensor = sensor.substring(0, n + 1)

                        var sensors;
                        if (pathToSensor in pathArray) {
                                sensors = pathArray[pathToSensor];
                        } else {
                                sensors = new Array();
                                pathArray[pathToSensor] = sensors;
                        }

                        sensors[sensors.length] = {full_path:sensor, name:sensor.substring(n + 1, sensor.length), description:jsonHomeObj[sensor]}
                }
        }

        //iterate array

        var tablearea = document.getElementById('tablearea'),
                table = document.createElement('table');
        table.className ="u-full-width";

        for (var path in pathArray) {

                var thead = document.createElement('thead');
                var tr = document.createElement('tr');
                var td = document.createElement('th');
                //td.style.width = '300px';
                tr.appendChild( td );
                td = document.createElement('th');
                //td.style.width = '200px';
                tr.appendChild( td );
                td = document.createElement('th');
                //td.style.width = '200px';
                tr.appendChild( td );
                tr.appendChild( document.createElement('th') );
                tr.cells[0].appendChild( document.createTextNode(path) );
                tr.cells[1].appendChild( document.createTextNode('') );
                tr.cells[2].appendChild( document.createTextNode('') );
                tr.cells[3].appendChild( document.createTextNode('') );
                thead.appendChild(tr);
                table.appendChild(thead);

                sensors = pathArray[path];
                for (i = 0; i < sensors.length; i++) {
                        var sensorName = sensors[i].name;
                        var pluginName = sensors[i].description.name;

                        if (jsonWebObj[pluginName] != undefined) {
                                var tbody = document.createElement('tbody');
                                var tr = document.createElement('tr');
                                tr.appendChild( document.createElement('td') );
                                tr.appendChild( document.createElement('td') );
                                tr.appendChild( document.createElement('td') );
                                tr.appendChild( document.createElement('td') );
                                tr.cells[0].appendChild( document.createTextNode('') )
                                tr.cells[1].appendChild( document.createTextNode(sensorName) ); 

                                var deviceType = jsonWebObj[pluginName]["device-type"];
                                var displayOptions = jsonWebObj[pluginName]["display-options"];
                                var displayString = "";

                                if (deviceType == "scene") {
                                        tr.cells[2].appendChild( document.createTextNode("SCENE") );
                                } else if (deviceType == "switch-simple") {
                                        var displayOption = displayOptions[0].option;
                                        var switchStatus = sensors[i].description[displayOption];
                                        var sensorFullPath = sensors[i].full_path;
                                        var buttonName = "buttonSimpleSwitch" + i;

                                        var span = document.createElement('span');
                                        span.innerHTML = '<input type="button" id="' + buttonName + '" value="' + switchStatus + '" onclick=simpleSwitch(\'' + sensorFullPath + '\',\'' + buttonName + '\')></input>';
                                        tr.cells[2].appendChild( span );
                                } else {
                                        var displayString = "";

                                        for (j = 0; j < displayOptions.length; j++) {
                                                var displayOption = displayOptions[j];
                                                var option = displayOption.option;
                                                var type = displayOption.type;
                                                var value1 = sensors[i].description[option];
                                                displayString = displayString + value1 + " ";
                                        }

                                        tr.cells[2].appendChild( document.createTextNode(displayString) );
                                }

                                var sensorFullPath = sensors[i].full_path;
                                var historyUrl = urlRoot + sensorFullPath + historySuffix;
                                var span = document.createElement('span');
                                span.innerHTML = '<button id="buttonHistory" value="History" onclick=generateHistory("' + historyUrl + '")>History</button>';
                                tr.cells[3].appendChild( span );
                                tbody.appendChild(tr);
                                table.appendChild(tbody);
                        }
                }

        }

        tablearea.appendChild(table);
};

getJSON(urlHome, 'get', function(jsonHomeObj) {
        getJSON(urlWeb, 'get', function(jsonWebObj) {
                displaySensors(jsonHomeObj, jsonWebObj);
        }, function(status) {
                alert('Something went wrong.');
        });
}, function(status) {
        alert('Something went wrong.');
});

