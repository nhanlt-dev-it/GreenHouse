const express = require('express');
const router = express.Router();
const {
  getGreenhouse1Data,
  postGreenhouse1Data,
  getGreenhouse2Data,
  postGreenhouse2Data,
} = require('../controllers/greenhouseControllers');

router.route('/api/data/greenhouse-1').get(getGreenhouse1Data).post(postGreenhouse1Data);
router.route('/api/data/greenhouse-2').get(getGreenhouse2Data).post(postGreenhouse2Data);

module.exports = router;
