const express = require('express');
const { exec } = require('child_process');
const path = require('path');
const app = express();
const port = 3000;

// Serve static files (like the HTML file)
app.use(express.static(path.join(__dirname, 'public')));

// Endpoint to list available tests
app.get('/list-tests', (req, res) => {
    const gtestBinary = path.resolve(__dirname, '/home/uma/workspace/Gzero/cmake/lib/GzeroUnitTest');

    // Execute the gtest binary to list all tests
    exec(`${gtestBinary} --gtest_list_tests`, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).send(`Error fetching test list: ${stderr}`);
        }

        const testList = stdout.split('\n').map(line => line.trim()).filter(line => line.length > 0);
        
        // Return the list of tests as a JSON object
        res.json(testList);
    });
});

// Endpoint to run the selected test(s)
app.get('/run-tests', (req, res) => {
    const gtestBinary = path.resolve(__dirname, '/home/uma/workspace/Gzero/cmake/lib/GzeroUnitTest');
    const testToRun = req.query.test || '';  // Get the test name from the query (e.g. /run-tests?test=TestCase.Test)

    // Construct the command to run the selected test or all tests
    const command = testToRun ? `${gtestBinary} --gtest_filter=${testToRun}` : gtestBinary;

    exec(command, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).send(`Test execution failed: ${stderr}`);
        }

        res.send(`Test Output: <pre>${stdout}</pre>`);
    });
});

// Start the server
app.listen(port, () => {
    console.log(`Server listening at http://localhost:${port}`);
});
