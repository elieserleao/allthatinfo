var Clay = require('pebble-clay');
var clayConfig = require('./config');
var clay = new Clay(clayConfig);
var settings = {};
var weatherUnit = "C";

//require = ('./weather');

var openweatherKey = '8ada84b14cf204ec183cc619238bac21';
//var wunderKey = '968769b570ed2aaa';
//var wunderKey = '';

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
  load_settings();
  
  navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
  
  Pebble.sendAppMessage({
    'TIMES_UPDWEATHER': settings.TIMES_UPDWEATHER,
    'TIMES_UPDSTEPS': settings.TIMES_UPDSTEPS,
    'HOURLY_VIBRATE': settings.HOURLY_VIBRATE,
    'HOURLY_VIBRATE_START': settings.HOURLY_VIBRATE_START,
    'HOURLY_VIBRATE_STOP': settings.HOURLY_VIBRATE_STOP   
  });  
});

Pebble.addEventListener('webviewclosed', function (e) {
  console.log('webview closed');
  console.log(e.type);
  console.log(e.response);
});

function load_settings(){
  settings = JSON.parse(localStorage.getItem('clay-settings')) ||
             JSON.parse('{"CLOCK_BGCOLOR":16777215,"CLOCK_COLOR":0,"HOURLY_VIBRATE":"S","HOURLY_VIBRATE_START":"08:00","HOURLY_VIBRATE_STOP":"00:00","TIMES_UPDWEATHER":"15","TIMES_UPDSTEPS":"5","WEATHER_APIKEY":"","WEATHER_UNIT":"C"}');
  
  weatherUnit = settings.WEATHER_UNIT;
  
  console.log("Settings: " + JSON.stringify(settings));
}

function request_weather_wunder(latitude, longitude){
  var req = new XMLHttpRequest();
  var ast = new XMLHttpRequest();
  
  console.log('Using WUnder API.');
 
  try {
    req.open('GET', 'http://api.wunderground.com/api/'+ settings.WEATHER_APIKEY + '/conditions/q/' + latitude + ',' + longitude + '.json', true);
    req.onload = function () {
      if (req.readyState === 4) {
        if (req.status === 200) {
          var response = JSON.parse(req.responseText);
          console.log(JSON.stringify(response));

          if (typeof(response.response.error) == "object"){
            Pebble.sendAppMessage({                 
              'WEATHER_WU': 1,
              'WEATHER_CITY': response.response.error.description,
              'WEATHER_ALTITUDE': " ",
              'WEATHER_UNIT': "X"
            });
            
            return;
          }
          
          if(!response.current_observation.weather){
            response.current_observation.weather = "Clear";
          }

          console.log(response.current_observation.temp_c);
          console.log(response.current_observation.weather);
          console.log(response.current_observation.display_location.city);

          Pebble.sendAppMessage({
            'WEATHER_WU': 1,
            'WEATHER_TEMP': temp_calc(response.current_observation.temp_c),
            'WEATHER_COND': response.current_observation.weather,
            'WEATHER_CITY': response.current_observation.display_location.city,
            'WEATHER_HUM': response.current_observation.relative_humidity,
            'WEATHER_WIND': process_wind(response.current_observation.wind_gust_kph / 3.6),
            'WEATHER_UNIT': weatherUnit
          });
        } else {  
          Pebble.sendAppMessage({
            'WEATHER_WU': 1,
            'WEATHER_CITY': 'Error: Api?'
          });
        }
      }
    };
    
    ast.open('GET', 'http://api.wunderground.com/api/'+ settings.WEATHER_APIKEY + '/astronomy/q/' + latitude + ',' + longitude + '.json', true);
    ast.onload = function () {
      if (ast.readyState === 4) {
        if (ast.status === 200) {                
          var astResp = JSON.parse(ast.responseText);
          console.log(JSON.stringify(astResp));

          console.log(astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute);

          Pebble.sendAppMessage({
            'WEATHER_WU': 1,
            'WEATHER_SUN': astResp.sun_phase.sunrise.hour + ':' + astResp.sun_phase.sunrise.minute + "-" + astResp.sun_phase.sunset.hour + ':' + astResp.sun_phase.sunset.minute
          });
        } else {   
          Pebble.sendAppMessage({
            'WEATHER_WU': 1,
            'WEATHER_CITY': 'Error: Api Ast?'
          });
        }
      }
    };

    ast.send(null);
    req.send(null);
  }catch(e){
    Pebble.sendAppMessage({
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
        
        // {"cod":"404","message":"Error: Not found city"}
        if(response.cod == "404"){
          Pebble.sendAppMessage({
            'WEATHER_OWM': 1,
            'WEATHER_CITY': response.message,
            'WEATHER_ALTITUDE': " "            
          });
          
          return;
        }

        var sr = new Date(response.sys.sunrise * 1000);
        var srStr = leftpad(sr.getHours(), 2, 0) + ':' + leftpad(sr.getMinutes(), 2, 0);
        
        var ss = new Date(response.sys.sunset * 1000);
        var ssStr = leftpad(ss.getHours(), 2, 0) + ':' + leftpad(ss.getMinutes(), 2, 0);
        
        Pebble.sendAppMessage({
          'WEATHER_OWM': 1,
          'WEATHER_TEMP': temp_calc(parseFloat(response.main.temp - 273.15).toFixed(1)),
          'WEATHER_SUN': srStr + "-" + ssStr,
          //'WEATHER_COND': response.weather[0].description,
          'WEATHER_COND': 'Scattered clouds',
          'WEATHER_CITY': response.name,
          'WEATHER_HUM': response.main.humidity + "%",
          'WEATHER_WIND': process_wind(response.wind.speed),
          'WEATHER_UNIT': weatherUnit
        });
      } else {  
        Pebble.sendAppMessage({
          'WEATHER_OWM': 1,
          'WEATHER_CITY': 'Error: Api?'
        });
      }
    }
  };
  
  req.send(null);
}

function process_wind(wind){
  var ws = parseFloat(wind);
  if(ws === 0)
    return "0";
  
  if(ws < 1){
    return String(ws.toFixed(1));
  }
  
  return String(Math.round(ws));
}

function fetchWeather(latitude, longitude) {  
  console.log('lat,lon=' + latitude + ',' + longitude);
  
  if(!settings.WEATHER_APIKEY) {
    request_weather_owm(latitude, longitude);
  }else{
    request_weather_wunder(latitude, longitude);
  }
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  
  if(pos.coords.altitude){
    cur_altitude = Math.round(pos.coords.altitude) + " m";
  }
  
  Pebble.sendAppMessage({
    'WEATHER_ALTITUDE': cur_altitude
  });
  
  fetchWeather(coordinates.latitude, coordinates.longitude);
  
  console.log("POS: " + JSON.stringify(pos));
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  
  Pebble.sendAppMessage({
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