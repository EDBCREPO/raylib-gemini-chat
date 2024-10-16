const http = require('node:http')

http.createServer((req, res) => {
	let body = '';
    
    req.on('data', chunk => {
        body += chunk.toString();
    });
    
    req.on('end', () => {
        console.log('Body:', body );

        res.setHeader('Content-Type', 'application/json');
        res.write(JSON.stringify({ token: "abcdefg" }));
        res.end();
    });
}).listen(8000)