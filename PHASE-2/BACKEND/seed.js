const mongoose = require('mongoose');
const dotenv = require('dotenv');
const bcrypt = require('bcryptjs');
const Student = require('./models/Student');
const Faculty = require('./models/Faculty');
const ExamHall = require('./models/ExamHall');
const Seating = require('./models/Seating');

dotenv.config();

const seedData = async () => {
  try {
    await mongoose.connect(process.env.MONGO_URI);
    console.log('Connected for seeding');

    const hashedStudentPassword = await bcrypt.hash('student123', 10);
    const hashedFacultyPassword = await bcrypt.hash('faculty123', 10);

    await Student.create({
      rollNo: '101',
      name: 'Test Student',
      semester: 3,
      section: 'D',
      subjects: ['DSA', 'OOP'],
      password: hashedStudentPassword
    });

    await Faculty.create({
      facultyId: 'F001',
      name: 'Test Faculty',
      department: 'CSE',
      email: 'faculty@test.com',
      password: hashedFacultyPassword
    });

    await ExamHall.create({
      hallId: 'H101',
      building: 'Block A',
      floor: 1,
      capacity: 60
    });

    await Seating.create({
      hallId: 'H101',
      rollNo: '101',
      seatNumber: 'A12'
    });

    console.log('Seed data added successfully');
    process.exit();
  } catch (error) {
    console.log('Seeding failed:', error.message);
    process.exit(1);
  }
};

seedData();