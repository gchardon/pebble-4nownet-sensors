var xhrRequest = function(url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function() {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function getSensorData(pos) {
    // Construct URL
    // TODO add UID list to app config
    var url = "http://www.4now.net/sasp/clavel/last/?uid=1,2,6,8,10";

    // Send request to 4now net
    xhrRequest(url, 'GET',
        function(responseText) {
            // responseText contains JSON object
            var data = JSON.parse(responseText);
            console.log("Received sensor data from 4now");
            
            var location = "";
            var value = "";
            var timestamp = Math.floor(Date.now() / 1000);            
            
            var nb_sensors = data.length;
            for (var i = 0; i < nb_sensors; i++) {
                location += data[i].location_label.split(" ", 1)[0] + "|";
                value += Math.round(data[i].value) + "|";
            }
            console.log("Sensing " + nb_sensors + " sensors data to the watch");
            
            // Assemble dictionary using our keys
            var dictionary = {
                "KEY_TIMESTAMP": timestamp,
                "KEY_SENSOR_VALUE": value,
                "KEY_SENSOR_LOCATION": location,
            };

            // Send to Pebble
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Sensor info sent to Pebble successfully!");
                },
                function(e) {
                    console.log("Error sending sensor info to Pebble!");
                }
            );
        }
    );
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready',
    function(e) {
        console.log("PebbleKit JS ready!");

        // Get the initial data
        getSensorData();
    }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
    function(e) {
        console.log("AppMessage received!");
        getSensorData();
    }
);