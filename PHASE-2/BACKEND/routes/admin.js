const express = require('express');
const Student = require('../models/Student');
const ExamHall = require('../models/ExamHall');
const Exam = require('../models/Exam');
const Seating = require('../models/Seating');
const UFM = require('../models/UFM');
const { protect, authorizeRoles } = require('../middleware/auth');

const router = express.Router();

// Add a student
router.post('/student', async (req, res) => {
  try {
    const student = await Student.create(req.body);
    res.status(201).json(student);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// Add an exam hall
router.post('/hall', async (req, res) => {
  try {
    const hall = await ExamHall.create(req.body);
    res.status(201).json(hall);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// Create an exam
router.post('/exam', async (req, res) => {
  try {
    const exam = await Exam.create(req.body);
    res.status(201).json(exam);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// Import seating data (from the C++ seat allocation output)
router.post('/import-seating', async (req, res) => {
  try {
    const seatingData = req.body.seatingData; // expects an array
    const result = await Seating.insertMany(seatingData);
    res.status(201).json({ message: 'Seating imported', count: result.length });
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

// View all UFM records
router.get('/ufm', async (req, res) => {
  try {
    const records = await UFM.find();
    res.json(records);
  } catch (error) {
    res.status(500).json({ message: error.message });
  }
});

module.exports = router;