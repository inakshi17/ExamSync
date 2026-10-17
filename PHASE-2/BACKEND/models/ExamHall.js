const mongoose = require('mongoose');

const examHallSchema = new mongoose.Schema({
  hallId: { type: String, required: true, unique: true },
  building: String,
  floor: Number,
  capacity: Number
});

module.exports = mongoose.model('ExamHall', examHallSchema);