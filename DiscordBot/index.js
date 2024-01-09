const { WebSocketServer } = require('ws')
const wss = new WebSocketServer({ port: 1479 });

wss.on('connection', (ws, req) => {
  ws.on('message', (message) => {
    console.log(message.toString());
  });

  // Handle WebSocket closing
  ws.on('close', () => {
    console.log("Connection closed!");
  });
});