var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);
var settings = {};
var weatherUnit = "C";

//require = ('./weather');

var openweatherKey = '8ada84b14cf204ec183cc619238bac21';
//var wunderKey = '968769b570ed2aaa';
var wunderKey = '';

var cur_altitude = 'N/A';

var locationOptions = {
  enableHighAccuracy: true,
  'timeout': 15000,
  'maximumAge': 60000
};

Pebble.addEventListener('appmessage', function (e) {
  var dict = e.payload;
  
//  if (dict['WEATHER_REQUEST']){
//    wunderKey = dict['WEATHER_REQUEST'];
//  }
  load_settings();

  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  
  console.log(e.type);
  console.log(JSON.stringify(e.payload));
  console.log('message!');
});

Pebble.addEventListener('ready', function (e) {
  console.log('JS ready!' + e.ready);
  //window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  Pebble.sendAppMessage({
    'JS_READY': '1',    
  });
  
  load_settings();
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});

function load_settings(){
  settings = JSON.parse(localStorage.getItem('clay-settings')) || {};
  
  wunderKey = settings.WEATHER_APIKEY;
  weatherUnit = settings.WEATHER_UNIT;
  
  console.log("Settings: " + JSON.stringify(settings));
}

function request_weather_wunder(latitude, longitude){
  var req = new XMLHttpRequest();
  var ast = new XMLHttpRequest();
  
  console.log('Using WUnder API.');
 
  try {
    req.open('GET', 'http://api.wunderground.com/api/'+ wunderKey + '/conditions/q/' + latitude + ',' + longitude + '.json', true);
    req.onload = function () {
      if (req.readyState === 4) {
        if (req.status === 200) {
          var response = JSON.parse(req.responseText);
          console.log(JSON.stringify(response));

          if (typeof(response.response.error) == "object"){
            /*Pebble.sendAppMessage({
              'WEATHER_REPLY': '1',                  
              'WEATHER_WU': 1,
              'WEATHER_TEMP': "00",
              'WEATHER_SUN': "unavailable",
              'WEATHER_COND': "Not good",
              'WEATHER_HUM': "0%",
              'WEATHER_WIND': "0",
              'WEATHER_CITY': response.response.error.description,
              'WEATHER_ALTITUDE': cur_altitude + " m"
            });
            */
            Pebble.sendAppMessage({
              'WEATHER_REPLY': '1',                  
              'WEATHER_WU': 1,
              'WEATHER_CITY': response.response.error.description,
              'WEATHER_ALTITUDE': " "
            });
            
            return;
          }
          
          ast.open('GET', 'http://api.wunderground.com/api/'+ wunderKey + '/astronomy/q/' + latitude + ',' + longitude + '.json', true);
          ast.onload = function () {
            if (ast.readyState === 4) {
              if (ast.status === 200) {                
                var astResp = JSON.parse(ast.responseText);
                console.log(JSON.stringify(astResp));
                
                if(!response.current_observation.weather){
                  response.current_observation.weather = "Clear";
                }
                
                console.log(response.current_observation.temp_c);
                console.log(astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute);
                console.log(response.current_observation.weather);
                console.log(response.current_observation.display_location.city);

                Pebble.sendAppMessage({
                  'WEATHER_REPLY': '1',
                  'WEATHER_WU': 1,
                  'WEATHER_TEMP': temp_calc(response.current_observation.temp_c),
                  'WEATHER_SUN': astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute,
                  'WEATHER_COND': response.current_observation.weather,
                  'WEATHER_CITY': response.current_observation.display_location.city,
                  'WEATHER_HUM': response.current_observation.relative_humidity,
                  'WEATHER_WIND': process_wind(response.current_observation.wind_gust_kph / 3.6)
                });
              } else {   
                Pebble.sendAppMessage({
                  'WEATHER_REPLY': '1',
                  'WEATHER_WU': 1,
                  'WEATHER_CITY': 'Error: Api?'
                });
              }
            }
          };
  
          ast.send(null);
        } else {  
          Pebble.sendAppMessage({
            'WEATHER_REPLY': '1',
            'WEATHER_WU': 1,
            'WEATHER_CITY': 'Error: No internet?'
          });
        }
      }
    };  
    req.send(null);
  }catch(e){
    Pebble.sendAppMessage({
      'WEATHER_REPLY': '1',
      'WEATHER_WU': 1,
      'WEATHER_CITY': 'Error: ' + e.message
    });
  }
}

function request_weather_owm(latitude, longitude){
  var req = new XMLHttpRequest();
  
  console.log('Using OWM API.');

  req.open('GET', 'http://api.openweathermap.org/data/2.5/weather?' + 'lat=' + latitude + '&lon=' + longitude + '&cnt=1&appid=' + openweatherKey, true);
  req.onload = function () {
    if (req.readyState === 4) {
      if (req.status === 200) {
        var response = JSON.parse(req.responseText);
          console.log(JSON.stringify(response));

        var sr = new Date(response.sys.sunrise * 1000);
        var srStr = leftpad(sr.getHours(), 2, 0) + ':' + leftpad(sr.getMinutes(), 2, 0);
        
        var ss = new Date(response.sys.sunset * 1000);
        var ssStr = leftpad(ss.getHours(), 2, 0) + ':' + leftpad(ss.getMinutes(), 2, 0);
        
        Pebble.sendAppMessage({
          'WEATHER_REPLY': '1',
          'WEATHER_OWM': 1,
          'WEATHER_TEMP': temp_calc(parseFloat(response.main.temp - 273.15).toFixed(1)),
          'WEATHER_SUN': srStr + "-" + ssStr,
          'WEATHER_COND': response.weather[0].description,
          'WEATHER_CITY': response.name,
          'WEATHER_HUM': response.main.humidity + "%",
          'WEATHER_WIND': process_wind(response.wind.speed)
        });
      } else {  
        Pebble.sendAppMessage({
          'WEATHER_REPLY': '1',
          'WEATHER_OWM': 1,
          'WEATHER_CITY': 'Error: Api?'
        });
      }
    }
  };
  
  req.send(null);
}

function process_wind(wind){
  var ws = Math.round(wind).toFixed(1);
  
  if(ws < 1){
    return String(ws);
  }else{
    return String(Math.round(ws));
  }
}

function fetchWeather(latitude, longitude) {  
  console.log('lat,lon=' + latitude + ',' + longitude);
  
  if(!wunderKey) {
    request_weather_owm(latitude, longitude);
  }else{
    request_weather_wunder(latitude, longitude);
  }
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
  
  console.log("POS: " + JSON.stringify(pos));
  
  if(pos.coords.altitude){
    cur_altitude = Math.round(pos.coords.altitude) + " m";
  }
  
  Pebble.sendAppMessage({
    'WEATHER_REPLY': '1',
    'WEATHER_ALTITUDE': cur_altitude
  });
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  
  Pebble.sendAppMessage({
    'WEATHER_REPLY': '1',
    'WEATHER_CITY': 'Error: Location?'
  });
}

function temp_calc(temp){
  if(weatherUnit === "F"){
    temp = (temp * 1.8) + 32;
  }
  
  var sp = String(temp).split(".");
  var res = sp[0];
  
  if(sp.length > 1){
    if(sp[1] >= 5){
      res++;
    }
  }
  
  return String(res);
}

function leftpad(str, len, ch) {
  str = String(str);
  var i = -1;
  if (!ch && ch !== 0) ch = ' ';
  len = len - str.length;
  while (++i < len) {
    str = ch + str;
  }
  return str;
}