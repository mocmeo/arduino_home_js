var pushpad = require('pushpad');

var AUTH_TOKEN = '5f4078a45534b008225884877e449718';
var PROJECT_ID = 4833;


var project = new pushpad.Pushpad({
  authToken: AUTH_TOKEN,
  projectId: PROJECT_ID
});


module.exports = {
    pushNotification: function(title, message) {
        var notification = new pushpad.Notification({
          project: project,
          body: message,//'ESP32 connected successfully!',
          title: title, //'IoT Home - Devices',
          targetUrl: 'http://192.168.0.102/html/',
          ttl: 2,
          requireInteraction: false
        });
        notification.broadcast(function(err, result) { /*...*/ });
    }
}
