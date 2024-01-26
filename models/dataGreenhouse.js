const mongoose = require('mongoose');

const dataGreenhouseSchema = new mongoose.Schema({
  greenhouseId: String,
  temperature: {
    value: Number,
    unit: String, // '°C'
  },
  humidity: {
    value: Number,
    unit: String, // '%'
  },
  soil_moisture: {
    value: Number,
    unit: String, // '%'
  },
  status_rain: String,
  status_light_sensor: String,
  status_led: String,
  status_fan: String,
  status_pump: String,
  status_door: String,
  roof: String,
  timestamp: { type: Date, default: Date.now },
});

const DataGreenhouse1 = mongoose.model('greenHouse1', dataGreenhouseSchema);
const DataGreenhouse2 = mongoose.model('greenHouse2', dataGreenhouseSchema);

module.exports = {
  DataGreenhouse1,
  DataGreenhouse2,
};
