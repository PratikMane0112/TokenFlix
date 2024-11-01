
# Computer Network Project with Socket Programming in C language

Architecture of socket programming -

![](https://www.c-sharpcorner.com/article/socket-programming-in-cpp-using-boost-asio-tcp-server-and-client/Images/sdgfh.jpg)

This project demonstrates a simple client-server architecture using socket programming in C. The server provides a list of stations to the client, and the client can choose a station to watch videos from. Each user has a certain number of tokens required to watch videos. When a user runs out of tokens, they must answer questions to gain more tokens.

## Server (`server.c`)

The server side of the project is implemented in the `server.c` file. It performs the following tasks:

- Initializes stations data from a file.
- Creates a socket, binds it, and listens for connections.
- Sends the list of available stations to clients upon connection.
- Receives username, password, and initial tokens from clients.
- Processes client requests for watching videos, updating tokens, and sending tokens to clients.
- Manages video playback using the `ffplay` command-line tool.

## Client (`client.c`)

The client side of the project is implemented in the `client.c` file. It performs the following tasks:

- Creates a socket and connects to the server.
- Receives the list of available stations from the server.
- Sends username, password, and initial tokens to the server.
- Allows users to choose a station to watch videos from.
- Receives the updated number of tokens from the server after each action.
- Prompts users to answer questions to gain more tokens when they run out.

## Instructions:

1. Compile the `server.c` and `client.c` files separately using a C compiler.
2. Run the server executable first.
3. Run multiple instances of the client executable to simulate multiple clients connecting to the server.
4. Follow the prompts on the client-side interface to interact with the server and watch videos.

## Notes:

- Update the file paths for video files (`movie.mp4`, `Kapil.mp4`, etc.) in both `server.c` and `client.c` according to your system configuration.
- Ensure that the `ffplay` command-line tool is installed on your system for video playback functionality.

