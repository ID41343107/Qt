const net = require('net');
const { Client, GatewayIntentBits } = require('discord.js');

// ===== 設定區 =====
const DISCORD_TOKEN = 496f23d456366492509e66d4a5b27f68a57c77ea6204b1307e5c926e7bcc9912;
const CHANNEL_ID = 1457798266279166076;
const TCP_PORT = 8888;
// ==================

const client = new Client({
    intents: [GatewayIntentBits.Guilds, GatewayIntentBits.GuildMessages]
});

client.once('ready', () => {
    console.log(`Discord Bot logged in as ${client.user.tag}`);
});

// TCP Server
const server = net.createServer((socket) => {
    socket.on('data', async (data) => {
        const message = data.toString().trim();
        console.log('Received:', message);

        if (message === '有人來') {
            const channel = await client.channels.fetch(CHANNEL_ID);
            if (channel) {
                channel.send('有人來');
            }
        }
    });
});

server.listen(TCP_PORT, '127.0.0.1', () => {
    console.log(`TCP Server listening on 127.0.0.1:${TCP_PORT}`);
});

client.login(DISCORD_TOKEN);
