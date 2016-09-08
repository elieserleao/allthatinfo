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
        "defaultValue": "Weather"
      },
      {
        "type": "input",
        "messageKey": "WEATHER_APIKEY",
        "label": "Weather Underground API Key",
        "description": "Configure your Weather Underground API key.<br/> Get yours <a href='https://www.wunderground.com/weather/api/'>here</a>.<br/><br/> Leave blank to use OpenWeatherMap."
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];