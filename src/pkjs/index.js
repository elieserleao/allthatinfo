var openweatherKey = '8ada84b14cf204ec183cc619238bac21';
var wunderKey = '968769b570ed2aaa';

var locationOptions = {
  'timeout': 15000,
  'maximumAge': 60000
};

Pebble.addEventListener('appmessage', function (e) {
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
  console.log(e.payload);
  console.log('message!');
});

Pebble.addEventListener('ready', function (e) {
  console.log('connect!' + e.ready);
  window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  console.log(e.type);
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});

function fetchWeather(latitude, longitude) {
  var req = new XMLHttpRequest();
  var ast = new XMLHttpRequest();
  
  console.log('lat,lon=' + latitude + ',' + longitude);
  
  req.open('GET', 'http://api.wunderground.com/api/'+ wunderKey + '/conditions/q/' + latitude + ',' + longitude + '.json', true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        var response = JSON.parse(req.responseText);
        
        ast.open('GET', 'http://api.wunderground.com/api/'+ wunderKey + '/astronomy/q/' + latitude + ',' + longitude + '.json', true);
        ast.onload = function () {
          if (ast.readyState === 4) {
            if (ast.status === 200) {
              var astResp = JSON.parse(ast.responseText);

              console.log(response.current_observation.temp_c);
              console.log(astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute);
              console.log(response.current_observation.weather);
              console.log(response.current_observation.display_location.city);
              
              Pebble.sendAppMessage({
                'WEATHER_REPLY': '1',
                'WEATHER_TEMP': String(response.current_observation.temp_c),
                'WEATHER_SUN': astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute,
                'WEATHER_COND': response.current_observation.weather,
                'WEATHER_CITY': response.current_observation.display_location.city
              });

            } else {
              console.log('Error AST');
            }
          }
        };

        ast.send(null);
      } else {
        console.log('Error CNDss');
      }
    }
  };  
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  
  Pebble.sendAppMessage({
    'WEATHER_REPLY': '1',
    'WEATHER_TEMP': 'N/A',
    'WEATHER_SUN': 'unavailable',
    'WEATHER_COND': 'Not Good',
    'WEATHER_CITY': 'Somewhere'
  });
}