# Face Recognition + Discord Notifier Integration

This project integrates a Qt-based face recognition system with a Discord notification service.

## Overview

The system detects faces using a camera and sends notifications to Discord when someone is detected:
- When any face is detected (recognized or unknown) → sends "有人來" (someone is here) to Discord
- The Discord bot forwards this notification to the configured Discord channel

## Components

### 1. Face Recognition Application (Qt/C++)
Located in `/face` directory

**Features:**
- Real-time face detection using OpenCV DNN models
- Face recognition with embedding-based matching
- User registration and deletion
- TCP socket client to communicate with Discord notifier
- Door access control based on recognition

**Key Files:**
- `mainwindow.cpp/h` - Main application logic
- `face.pro` - Qt project file

**Requirements:**
- Qt 6.x with GUI, SQL, and Network modules
- OpenCV 4.5.5 with DNN module
- SQLite database for user storage

### 2. Discord Notifier Service (Node.js)
Located in `/discord-notifier/discord-notifier` directory

**Features:**
- TCP server listening on port 8888
- Discord bot integration using discord.js
- Handles message: "有人來" (someone is coming)

**Key Files:**
- `index.js` - Main server logic
- `package.json` - Node.js dependencies
- `.env` - Configuration (Discord token, channel ID, port)

**Requirements:**
- Node.js
- discord.js v14.x
- dotenv

## Setup Instructions

### Discord Notifier Setup

1. Navigate to the discord-notifier directory:
   ```bash
   cd discord-notifier/discord-notifier
   ```

2. Install dependencies (if not already installed):
   ```bash
   npm install
   ```

3. Configure `.env` file with your Discord credentials:
   ```
   DISCORD_TOKEN=your_discord_bot_token
   CHANNEL_ID=your_channel_id
   PORT=8888
   HOST=127.0.0.1
   ```

4. Start the Discord notifier service:
   ```bash
   node index.js
   ```

### Face Recognition Application Setup

1. Ensure OpenCV 4.5.5 with DNN module is installed at the path specified in `face.pro`

2. Place the required model files in the correct directory:
   - `deploy.prototxt`
   - `res10_300x300_ssd_iter_140000.caffemodel`
   - `openface_nn4.small2.v1.t7`

3. Build and run the Qt application using Qt Creator or command line

4. Register users as needed for face recognition

## How It Works

1. **Face Detection**: The application continuously captures frames from the camera and uses a DNN-based face detector to locate faces

2. **Face Recognition**: When a face is detected, it extracts face embeddings and compares them with stored user embeddings in the SQLite database

3. **Discord Notification**: 
   - When any face is detected (recognized or unknown), the application sends "有人來" (someone is here) via TCP socket
   
4. **Notification Tracking**: The system tracks the last notified person to avoid sending duplicate notifications for the same person

5. **Discord Relay**: The Discord notifier receives the TCP message and posts it to the configured Discord channel

## Communication Protocol

The Qt application and Discord notifier communicate via TCP socket:
- **Protocol**: Simple line-based text protocol
- **Host**: 127.0.0.1 (localhost)
- **Port**: 8888
- **Format**: UTF-8 encoded text messages ending with newline (`\n`)
- **Supported Messages**: "有人來"

## Notes

- The application connects to the Discord notifier on startup
- If the connection fails, it will attempt to reconnect when sending messages
- Face recognition threshold is set to 0.6 (Euclidean distance)
- The door remains unlocked for 3 seconds after successful recognition
