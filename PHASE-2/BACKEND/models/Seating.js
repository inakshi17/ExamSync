const mongoose = require('mongoose');

const seatingSchema = new mongoose.Schema({
  examId: { type: mongoose.Schema.Types.ObjectId, ref: 'Exam' },
  hallId: String,
  rollNo: String,
  seatNumber: String
});

module.exports = mongoose.model('Seating', seatingSchema);