require("dotenv").config();
const net = require("net");
const { Client, GatewayIntentBits } = require("discord.js");

const DISCORD_TOKEN = process.env.DISCORD_TOKEN;
const CHANNEL_ID = process.env.CHANNEL_ID;
const HOST = process.env.HOST || "127.0.0.1";
const PORT = Number(process.env.PORT || 8888);

if (!DISCORD_TOKEN) {
  console.error("Missing DISCORD_TOKEN in .env");
  process.exit(1);
}
if (!CHANNEL_ID) {
  console.error("Missing CHANNEL_ID in .env");
  process.exit(1);
}

const client = new Client({
  intents: [GatewayIntentBits.Guilds],
});

client.once("ready", () => {
  console.log(`Discord logged in as: ${client.user.tag}`);
});

async function sendToDiscord(text) {
  const channel = await client.channels.fetch(CHANNEL_ID);
  await channel.send(text);
}

const server = net.createServer((socket) => {
  let buffer = "";

  socket.on("data", async (data) => {
    buffer += data.toString("utf8");

    let idx;
    while ((idx = buffer.indexOf("\n")) !== -1) {
      const line = buffer.slice(0, idx).trim();
      buffer = buffer.slice(idx + 1);

      console.log("TCP received:", line);

      if (line === "有人來") {
        await sendToDiscord("有人來");
      } else if (line === "熟人") {
        await sendToDiscord("熟人");
      } else if (line === "陌生人") {
        await sendToDiscord("陌生人");
      }
    }
  });
});

server.listen(PORT, HOST, () => {
  console.log(`TCP server listening: ${HOST}:${PORT}`);
});

client.login(DISCORD_TOKEN);
