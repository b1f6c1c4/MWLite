var express = require("express");
var redis = require("redis");
var bodyParser = require("body-parser");

var app = express();

// all environments
app.set("port", 23456);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

var router = express.Router();
var client = redis.createClient(process.env.REDIS_HOST || "localhost", 6379);

client.on("connect", function () {
    console.log("Redis connected");
});

router.get("/", function (req, res) {
    res.json({ "state": "ok" });
});

router.post("/push", function (req, res) {
    var grp = req.query.r;
    var result = 0;
    (function next(id) {
        if (id == grp) {
            res.json({ "status": result });
            return;
        }
        client.rpush("MWLiteWorkLoad", req.body, function (rst) {
            // TODO
            next(id + 1);
        });
    })(0);
});

app.use("/", router);

var server = app.listen(app.get("port"), function () {
    console.log("Express server listening on port " + app.get("port"));
});
