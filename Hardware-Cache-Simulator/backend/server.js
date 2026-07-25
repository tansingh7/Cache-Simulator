const express = require("express");
const cors = require("cors");
const { execFile } = require("child_process");
const path = require("path");

const app = express();

app.use(cors());
app.use(express.json());

const executable = path.join(__dirname, "..", "test0");

app.post("/run", (req, res) => {
    console.log("Configuration Received:");
    console.log(req.body);

    execFile(executable, { maxBuffer: 100 * 1024 * 1024 }, (error, stdout, stderr) => {

        if (error) {
            console.error(error);
            return res.status(500).json({
                error: stderr || error.message
            });
        }

        try {
            JSON.parse(stdout);
            res.type("application/json").send(stdout);
        } catch (e) {
            console.error(stdout);

            res.status(500).json({
                error: "Invalid JSON",
                output: stdout
            });
        }
    });
});

app.listen(5001, () => {
    console.log("Server running on http://localhost:5001");
});
