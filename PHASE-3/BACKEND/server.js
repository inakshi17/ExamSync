const express = require('express');
const dotenv = require('dotenv');
const mongoose = require('mongoose');
const healthRoutes = require('./routes/health');
const requestLogger = require('./middleware/requestLogger');
const { notFound, errorHandler } = require('./middleware/errorHandler');

dotenv.config();

const app = express();
app.use(express.json());
app.use(requestLogger);

// Health check: GET /health
app.use('/health', healthRoutes);

// Admin, auth, faculty and student routes are in PHASE-2/BACKEND/routes

// Error handling must come after all routes
app.use(notFound);
app.use(errorHandler);

const PORT = process.env.PORT || 5000;

mongoose
  .connect(process.env.MONGO_URI)
  .then(() => {
    console.log('MongoDB connected');
    app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
  })
  .catch((err) => {
    console.error('Database connection failed:', err.message);
    process.exit(1);
  });
