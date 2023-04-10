var express = require("express");

var router = express.Router();

router.get("/", function(req,res) {
    console.log("Root page!");
});

module.exports = router;