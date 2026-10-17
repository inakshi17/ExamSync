const mongoose = require('mongoose');

const facultySchema = new mongoose.Schema({
  facultyId: { type: String, required: true, unique: true },
  name: { type: String, required: true },
  department: String,
  email: String,
  password: String
});

module.exports = mongoose.model('Faculty', facultySchema);