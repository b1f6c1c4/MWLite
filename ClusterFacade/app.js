var express = require("express");
var redis = require("redis");
var bodyParser = require("body-parser");

var app = express();

// all environments
app.set("port", 23456);
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

var router = express.Router();
var client = redis.createClient("redis://" + (process.env.REDIS_HOST || "localhost") + ":6379");

client.on("connect", function () {
    console.log("Redis connected");
});

router.get("/", function (req, res) {
    res.json({ "status": "ok" });
});

router.get("/t", function (req, res) {
    var hsh = req.query.hash;
    client.get("t:" + hsh, function (err, rst) {
        if (err) {
            console.log(err);
            res.json(err);
        } else {
            res.json(rst);
        }
    });
});

router.get("/result", function (req, res) {
    var hsh = req.query.hash;
    client.mget([hsh + ":0", hsh + ":1"], function (err, rst) {
        if (err) {
            console.log(err);
            res.json(err);
        } else {
            res.json(rst);
        }
    });
});

router.post("/t", function (req, res) {
    var hsh = req.query.hash;
    client.set("t:" + hsh, req.body.t, function (err) {
        if (err) {
            console.log(err);
            res.json(err);
        } else {
            res.json({ "status": "ok" });
        }
    });
});

router.post("/result", function (req, res) {
    var hsh = req.query.hash;
    (function next(id) {
        if (id == req.body.length) {
            res.json({ "status": "ok" });
            return;
        }
        client.incrby(hsh + ":" + id, req.body[id], function (err) {
            if (err) {
                console.log(err);
                res.json(err);
            } else {
                next(id + 1);
            }
        });
    })(0);
});

router.post("/lpush", function (req, res) {
    var grp = parseInt(req.query.r);
    var obj;
    if (Array.isArray(req.body)) {
        obj = new Array();
        for (var i = 0; i < req.body.length; i++) {
            obj.push(JSON.stringify(req.body[i]));
        }
        console.log("lpush " + obj.length);
    } else {
        obj = JSON.stringify(req.body);
        console.log("lpush " + obj);
    }
    (function next(id) {
        if (id == grp) {
            res.json({ "status": "ok" });
            return;
        }
        client.lpush("MWLiteWorkLoad", obj, function (err) {
            if (err) {
                console.log(err);
                res.json(err);
            } else {
                next(id + 1);
            }
        });
    })(0);
});

router.post("/push", function (req, res) {
    var grp = parseInt(req.query.r);
    var obj;
    if (Array.isArray(req.body)) {
        obj = new Array();
        for (var i = 0; i < req.body.length; i++) {
            obj.push(JSON.stringify(req.body[i]));
        }
        console.log("push " + obj.length);
    } else {
        obj = JSON.stringify(req.body);
        console.log("push " + obj);
    }
    (function next(id) {
        if (id == grp) {
            res.json({ "status": "ok" });
            return;
        }
        client.rpush("MWLiteWorkLoad", obj, function (err) {
            if (err) {
                console.log(err);
                res.json(err);
            } else {
                next(id + 1);
            }
        });
    })(0);
});

app.use("/", router);

var server = app.listen(app.get("port"), function () {
    console.log("Express server listening on port " + app.get("port"));
});
