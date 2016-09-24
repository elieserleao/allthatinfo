module.exports = [  
  { 
    "type": "heading", 
    "defaultValue": "All That Info" 
  }, 
  { 
    "type": "text", 
    "defaultValue": "Configure everything you want here." 
  },
  {  
  "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Colors"
      },
      {
        "type": "color",
        "messageKey": "CLOCK_BGCOLOR",
        "defaultValue": "ffffff",
        "label": "Clock Background",
        "sunlight": false,
        "allowGray": true
      },
      {
        "type": "color",
        "messageKey": "CLOCK_COLOR",
        "defaultValue": "000000",
        "label": "Clock Foreground",
        "sunlight": false,
        "allowGray": true
      }
    ]
  },
  {  
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Hourly Vibrate"
      },
      {
        "type": "select",
        "messageKey": "HOURLY_VIBRATE",
        "defaultValue": "S",
        "label": "Vibe at",
        "description": "Enable or disable hourly vibrates",
        "options": [
          { 
            "label": "Always", 
            "value": "A"
          },
          { 
            "label": "When NOT sleeping", 
            "value": "S"
          },
          { 
            "label": "User defined", 
            "value": "U"
          },
          { 
            "label": "Never", 
            "value": "N"
          }
        ]
      },
      {
        "type": "input",
        "messageKey": "HOURLY_VIBRATE_START",
        "label": "Start vibe hour",
        "defaultValue": "08:00",
        "attributes": {
          "type": "time"
        }
      },
      {
        "type": "input",
        "messageKey": "HOURLY_VIBRATE_STOP",
        "label": "Vibe Stop Hour",
        "defaultValue": "00:00",
        "description": "Vibrate between these hours",
        "attributes": {
          "type": "time"
        }
      }
    ]
  },
  {  
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Update Times"
      },
      {
        "type": "select",
        "messageKey": "TIMES_UPDWEATHER",
        "defaultValue": 15,
        "label": "Weather",
        "description": "Update weather every x minutes",
        "options": [
          { 
            "label": "5", 
            "value": 5
          },
          { 
            "label": "10", 
            "value": 10
          },
          { 
            "label": "15", 
            "value": 15
          },
          { 
            "label": "30", 
            "value": 30
          },
          { 
            "label": "60", 
            "value": 60
          }
        ]
      },
      {
        "type": "select",
        "messageKey": "TIMES_UPDSTEPS",
        "defaultValue": 5,
        "label": "Steps",
        "description": "Update steps every x minutes",
        "options": [
          { 
            "label": "5", 
            "value": 5
          },
          { 
            "label": "10", 
            "value": 10
          },
          { 
            "label": "15", 
            "value": 15
          },
          { 
            "label": "30", 
            "value": 30
          },
          { 
            "label": "60", 
            "value": 60
          }
        ]
      }
    ]
  },
  {  
  "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Weather"
      },
      {
        "type": "input",
        "messageKey": "WEATHER_APIKEY",
        "label": "Weather Underground API Key",
        "description": "Configure your Weather Underground API key.<br/> Get yours <a href='https://www.wunderground.com/weather/api/'>here</a>.<br/><br/> Leave blank to use OpenWeatherMap."
      },
      {
        "type": "radiogroup",
        "messageKey": "WEATHER_UNIT",
        "label": "Weather Unit",
        "defaultValue": "C",
        "options": [
          { 
            "label": "Celsius", 
            "value": "C" 
          },
          { 
            "label": "Fahrenheit", 
            "value": "F" 
          }
    ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];