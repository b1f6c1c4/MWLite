var express = require("express");
var bodyParser = require("body-parser");

var app = express();

// all environments
app.set("port", 23456);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

var router = express.Router();

router.get("/", function (req, res) {
    res.json({ "state": "ok" });
});

app.use("/", router);

var server = app.listen(app.get("port"), function () {
    console.log("Express server listening on port " + app.get("port"));
});
