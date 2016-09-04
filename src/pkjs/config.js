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
        "label": "Weather Underground API Key"
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];