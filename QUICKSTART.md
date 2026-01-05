# Quick Start Guide

## Setting Up the Integration

### Step 1: Start the Discord Notifier

1. Open a terminal and navigate to the Discord notifier directory:
   ```bash
   cd discord-notifier/discord-notifier
   ```

2. Ensure the `.env` file is configured with your Discord credentials:
   ```
   DISCORD_TOKEN=your_discord_bot_token_here
   CHANNEL_ID=your_discord_channel_id_here
   PORT=8888
   HOST=127.0.0.1
   ```

3. Start the notifier service:
   ```bash
   node index.js
   ```

   You should see:
   ```
   TCP server listening: 127.0.0.1:8888
   Discord logged in as: YourBotName#1234
   ```

### Step 2: Register "DC" User

1. Build and run the Qt face recognition application

2. In the application window:
   - Type "DC" in the name field
   - Position your face in front of the camera
   - Click the "Register" button
   - Wait for confirmation message

### Step 3: Test the Integration

1. With both the Discord notifier and face app running:
   - Show your face (registered as "DC") to the camera
   - Check Discord channel - you should see: **"熟人"** (acquaintance)
   
2. Register another user with a different name (e.g., "Alice"):
   - Show that person's face to the camera
   - Check Discord channel - you should see: **"陌生人"** (stranger)

3. Show an unregistered face to the camera:
   - Check Discord channel - you should see: **"陌生人"** (stranger)

## Expected Behavior

| Face Detection | Discord Message |
|---------------|-----------------|
| "DC" recognized | 熟人 (acquaintance) |
| Other registered user | 陌生人 (stranger) |
| Unknown face | 陌生人 (stranger) |
| No face detected | (no message, resets state) |

## Troubleshooting

### Discord notifier not connecting
- Check if Node.js is installed: `node --version`
- Verify Discord credentials in `.env` file
- Ensure port 8888 is not in use: `netstat -an | grep 8888`

### Qt application can't connect to notifier
- Make sure Discord notifier is running first
- Check console output for "Connected to Discord notifier"
- Verify firewall settings allow localhost connections

### No notifications received
- Check if faces are being detected (green rectangle around face)
- Verify user is properly registered in database
- Check Qt application console for "Sent to Discord:" messages
- Check Discord notifier console for "TCP received:" messages

## Notes

- The system only sends a new notification when the detected person changes
- When no face is detected, the notification state resets
- This prevents spam notifications for the same person
- All communication happens locally on your machine (127.0.0.1)
