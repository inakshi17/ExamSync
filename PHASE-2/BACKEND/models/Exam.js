const mongoose = require('mongoose');

const examSchema = new mongoose.Schema({
  subjectCode: { type: String, required: true },
  examDate: Date,
  examTime: String,
  hallsAssigned: [String]
});

module.exports = mongoose.model('Exam', examSchema);