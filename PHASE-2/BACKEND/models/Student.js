const mongoose = require('mongoose');

const studentSchema = new mongoose.Schema({
  rollNo: { type: String, required: true, unique: true },
  name: { type: String, required: true },
  course: String,
  branch: String,
  semester: Number,
  section: String,
  subjects: [String],
  password: String
});

module.exports = mongoose.model('Student', studentSchema);