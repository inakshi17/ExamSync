const mongoose = require('mongoose');

const ufmSchema = new mongoose.Schema({
  examId: { type: mongoose.Schema.Types.ObjectId, ref: 'Exam' },
  rollNo: String,
  facultyId: String,
  description: String,
  timestamp: { type: Date, default: Date.now }
});

module.exports = mongoose.model('UFM', ufmSchema);