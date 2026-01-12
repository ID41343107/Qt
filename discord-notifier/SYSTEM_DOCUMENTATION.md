# Discord Notifier System Documentation

## Overview

The Discord Notifier is a Node.js-based bridge application that connects TCP-based systems to Discord messaging platform. It listens for messages on a TCP socket and forwards them to a designated Discord channel, enabling real-time notifications from local applications to Discord.

## System Architecture

### High-Level Workflow

```
External Application → TCP Socket (8888) → Discord Notifier → Discord API → Discord Channel
```

The system operates as a middleware service that:
1. Maintains a persistent connection to Discord as a bot
2. Listens on a TCP socket for incoming connections
3. Processes incoming messages from TCP clients
4. Forwards specific messages to a configured Discord channel

## Main Modules

### 1. Configuration Module

**File**: Environment variables loaded via `dotenv`  
**Responsibility**: Manages system configuration through environment variables

**Key Configuration Parameters**:
- `DISCORD_TOKEN`: Discord bot authentication token (required)
- `CHANNEL_ID`: Target Discord channel ID for notifications (required)
- `HOST`: TCP server host address (default: 127.0.0.1)
- `PORT`: TCP server port number (default: 8888)

**Initialization Process**:
- Loads environment variables from `.env` file at startup
- Validates required configuration (DISCORD_TOKEN, CHANNEL_ID)
- Exits with error if critical configuration is missing

### 2. Discord Client Module

**Implementation**: `discord.js` Client with Gateway Intents  
**Responsibility**: Manages Discord bot connection and authentication

**Key Components**:
- **Client Instance**: Discord.js client configured with `GatewayIntentBits.Guilds` intent
- **Authentication**: Logs in using the provided Discord bot token
- **Ready Event Handler**: Confirms successful connection and logs bot user tag

**Lifecycle**:
1. Client is instantiated with required gateway intents
2. `ready` event listener is registered for connection confirmation
3. Login process is initiated with the Discord token
4. Once connected, the bot becomes available for sending messages

### 3. Message Sender Module

**Function**: `sendToDiscord(text)`  
**Responsibility**: Sends messages to the configured Discord channel

**Process Flow**:
1. Fetches the target channel object using `CHANNEL_ID`
2. Sends the provided text message to the channel
3. Operates asynchronously to handle network latency

**Error Handling**: Relies on Discord.js built-in error handling for channel access and message delivery

### 4. TCP Server Module

**Implementation**: Node.js `net` module TCP server  
**Responsibility**: Listens for TCP connections and processes incoming messages

**Key Components**:
- **Server Instance**: TCP server bound to configured HOST:PORT
- **Socket Handler**: Manages individual client connections
- **Buffer Management**: Accumulates incoming data and processes line-by-line
- **Message Parser**: Extracts complete messages separated by newline characters

**Message Processing**:
1. Client connects to the TCP server
2. Server receives data chunks from the socket
3. Data is appended to a buffer (UTF-8 encoded)
4. Buffer is scanned for newline characters (`\n`)
5. Complete lines are extracted and trimmed
6. Messages are logged to console
7. Specific messages trigger Discord notifications

**Supported Messages**:
- `有人來` (Chinese: "Someone is here") - Triggers Discord notification

## Module Interactions

### Startup Sequence

```
1. Configuration Module
   ↓
2. Discord Client Module (initialization)
   ↓
3. TCP Server Module (initialization)
   ↓
4. Discord Client Module (login)
   ↓
5. System Ready (both services active)
```

### Message Flow

```
1. TCP Client sends data
   ↓
2. TCP Server receives data into buffer
   ↓
3. Buffer parser extracts complete message
   ↓
4. Message logged to console
   ↓
5. Message matched against trigger patterns
   ↓
6. Message Sender invoked (if matched)
   ↓
7. Discord Client sends to channel
   ↓
8. Discord API delivers to channel
```

## Data Flow

### Inbound Path (TCP → Discord)

1. **TCP Socket**: Receives raw byte stream from client
2. **Buffer Accumulation**: Data converted to UTF-8 string and accumulated
3. **Line Parsing**: Messages extracted at newline boundaries
4. **Pattern Matching**: Messages compared against trigger patterns
5. **Discord Forwarding**: Matched messages sent to Discord channel

### Configuration Data

- **At Startup**: Environment variables loaded from `.env` file
- **During Runtime**: Configuration is immutable (requires restart to change)

## Network Protocols

### TCP Protocol

- **Connection Type**: Server socket, accepts multiple clients
- **Message Format**: Line-based protocol (newline-delimited)
- **Encoding**: UTF-8
- **Connection Handling**: Persistent connections with event-driven data processing

### Discord Protocol

- **API**: Discord.js v14 library wrapping Discord Gateway API
- **Connection Type**: WebSocket-based persistent connection
- **Authentication**: Bot token authentication
- **Intents**: Guilds intent for basic server access

## Error Handling

### Configuration Errors

- **Missing DISCORD_TOKEN**: Fatal error, exits with code 1
- **Missing CHANNEL_ID**: Fatal error, exits with code 1
- **Invalid PORT/HOST**: Uses default values

### Runtime Errors

- **Discord Connection Failures**: Handled by discord.js (will throw exceptions)
- **TCP Server Errors**: Server will emit error events (not explicitly handled in current implementation)
- **Message Send Failures**: Async errors from Discord API propagate naturally

## Dependencies

### Production Dependencies

- **discord.js** (v14.25.1): Discord API client library
  - Provides high-level interface to Discord Gateway API
  - Handles WebSocket connections, authentication, and message sending
  
- **dotenv** (v17.2.3): Environment variable loader
  - Loads `.env` file into `process.env`
  - Used for configuration management

### Node.js Built-in Modules

- **net**: TCP server implementation
- **process**: Environment variables and process control

## Security Considerations

### Sensitive Data

- **Discord Token**: Stored in `.env` file (should not be committed to version control)
- **Channel ID**: Stored in `.env` file (less sensitive but should be configurable)

### Network Security

- **TCP Server**: Binds to localhost by default (127.0.0.1)
- **No Authentication**: TCP connections are not authenticated
- **No Encryption**: TCP traffic is unencrypted (plain text)

### Recommendations

1. Keep `.env` file out of version control (use `.gitignore`)
2. Restrict TCP server to localhost unless explicitly needed
3. Consider implementing TCP client authentication for production use
4. Use firewall rules to limit access to TCP port

## Usage Scenarios

### Primary Use Case: Face Recognition System Integration

The discord-notifier is designed to work with the Qt Face Recognition System in the same repository:

1. Face recognition system detects a person
2. Face recognition system opens TCP connection to localhost:8888
3. Face recognition system sends "有人來\n" message
4. Discord notifier receives and forwards to Discord channel
5. Discord channel notifies configured users/roles

### Alternative Use Cases

- Any application can send notifications by connecting to the TCP server
- Custom message patterns can be added to `index.js`
- Multiple applications can connect simultaneously

## Maintenance and Extension

### Adding New Message Triggers

To support additional message types, modify the message processing logic in `index.js`:

```javascript
if (line === "有人來") {
  await sendToDiscord("有人來");
} else if (line === "your-new-trigger") {
  await sendToDiscord("Your custom message");
}
```

### Changing Configuration

Edit the `.env` file and restart the application:

```bash
DISCORD_TOKEN=your-bot-token
CHANNEL_ID=your-channel-id
PORT=8888
HOST=127.0.0.1
```

### Monitoring

- Console logs show TCP received messages
- Discord connection status logged on startup
- Consider adding structured logging for production

## Limitations

1. **Single Channel**: Only sends to one Discord channel
2. **Fixed Triggers**: Message triggers are hard-coded
3. **No Message Queue**: Messages sent synchronously, no retry logic
4. **No Authentication**: TCP connections not authenticated
5. **Basic Error Handling**: Limited error recovery mechanisms

## Future Enhancement Opportunities

- Multi-channel support with routing based on message type
- Configurable message triggers via configuration file
- Message queue with retry logic for failed sends
- TCP client authentication mechanism
- Structured logging with log levels
- Health check endpoint
- Metrics and monitoring integration
- Message formatting and embedding support
- Rate limiting to prevent Discord API abuse
