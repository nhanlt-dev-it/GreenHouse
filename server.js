const express = require('express');
const dotenv = require('dotenv');
const connectDatabase = require('./config/database');
const errorMiddleware = require('./middlewares/errorMiddleware');
const greenhouseRoutes = require('./routes/greenhouseRoutes');

// Load environment variables
dotenv.config();

// Connect to MongoDB
connectDatabase();

const app = express();

// Middleware
app.use(express.json());

// Error handling middleware
app.use(errorMiddleware);

// Routes
app.use(greenhouseRoutes);

// Root path route
app.get('/', (req, res) => {
  res.send('Welcome to the smart greenhouse server system!');
});

// Start the server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
  console.log(`Server is running on port ${PORT}`);
});