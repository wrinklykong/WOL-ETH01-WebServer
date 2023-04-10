var express = require("express");
var path = require("path");

var app = express();

app.set("port", process.env.PORT || 3000);

var routes = require("./routes");
app.use(routes);

app.listen(app.get("port"), function() {
    console.log("Server started on port: "+app.get("port"));
});