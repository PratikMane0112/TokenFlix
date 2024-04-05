#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

#include <sys/types.h>

#include <signal.h>

#include <sys/wait.h>

#define MAX_STATIONS 9

typedef struct

{

    char name[50];

    int num_videos;

} Station;

int ffplay_pid = -1;

void send_station_list(int client_socket, Station stations[], int num_stations)

{

    char station_list[1800] = "";

    for (int i = 0; i < num_stations; i++)

    {

        snprintf(station_list + strlen(station_list), sizeof(station_list) - strlen(station_list),

                 "%d. %s (%d videos)\n", i + 1, stations[i].name, stations[i].num_videos);
    }

    if (send(client_socket, station_list, strlen(station_list), 0) == -1)

    {

        perror("Error sending station list");
    }
}

void displayVideo(const char *videoFilePath)

{

    if (ffplay_pid != -1)

    {

        // Kill the previous ffplay process

        kill(ffplay_pid, SIGTERM);

        waitpid(ffplay_pid, NULL, 0);

        ffplay_pid = -1;
    }

    // Create a new process

    pid_t pid = fork();

    if (pid < 0)

    {

        perror("Fork failed");
    }

    else if (pid == 0)

    {

        // This is the child process

        char command[256];

        snprintf(command, sizeof(command), "ffplay %s", videoFilePath);

        int status = system(command);

        if (status != 0)

        {

            printf("Error: ffplay failed to play the video.\n");
        }

        exit(0);
    }

    else

    {

        // This is the parent process

        ffplay_pid = pid;
    }
}

void updateUserTokens(const char *username, const char *password, int newTokens)

{

    FILE *file = fopen("user_tokens.txt", "r");

    FILE *tempFile = fopen("temp_user_tokens.txt", "w");

    if (file == NULL || tempFile == NULL)

    {

        perror("Error opening files");

        exit(1);
    }

    char fileUsername[50];

    char filePassword[50];

    int tokens;

    int found = 0;

    while (fscanf(file, "%s %s %d", fileUsername, filePassword, &tokens) == 3)

    {

        if (strcmp(fileUsername, username) == 0 && strcmp(filePassword, password) == 0)

        {

            fprintf(tempFile, "%-15s %-15s %-15d\n", username, password, newTokens);

            found = 1;
        }

        else

        {

            fprintf(tempFile, "%-15s %-15s %-15d\n", fileUsername, filePassword, tokens);
        }
    }

    fclose(file);

    fclose(tempFile);

    // Replace the original file with the temporary file

    remove("user_tokens.txt");

    rename("temp_user_tokens.txt", "user_tokens.txt");

    if (!found)

    {

        // If the user is not found, you can choose to handle it here, e.g., display an error message.

        printf("User not found\n");
    }
}

void sendTokensToClient(int client_socket, int tokens)

{

    if (send(client_socket, &tokens, sizeof(tokens), 0) == -1)

    {

        perror("Error sending tokens to client");
    }
}

int main()

{

    char username[50];

    char password[50];

    static int tokens;

    // Initialize stations data from a file

    int buffer[10];

    Station stations[MAX_STATIONS];

    FILE *dataFile = fopen("station_data.txt", "r");

    if (dataFile == NULL)

    {

        perror("Error opening data file");

        exit(1);
    }

    for (int i = 0; i < MAX_STATIONS; i++)

    {

        if (fscanf(dataFile, "%s %d", stations[i].name, &stations[i].num_videos) != 2)

        {

            perror("Error reading data from the file");

            fclose(dataFile);

            exit(1);
        }
    }

    fclose(dataFile);

    // Create a socket

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)

    {

        perror("Socket creation failed");

        exit(1);
    }

    // Bind the socket

    struct sockaddr_in server_addr, client_addr;

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(8080);

    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)

    {

        perror("Binding failed");

        exit(1);
    }

    // Listen for connectionsBinding failed: Invalid argument

    if (listen(server_socket, 5) == -1)

    {

        perror("Listening failed");

        exit(1);
    }

    printf("Server is listening...\n");

    while (1)

    {

        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket == -1)

        {

            perror("Accept failed");

            exit(1);
        }

        printf("Client connected.\n");

        send_station_list(client_socket, stations, MAX_STATIONS);

        // Receive username from client

        recv(client_socket, username, sizeof(username), 0);

        username[strcspn(username, "\n")] = '\0';

        printf("Received Username: %s\n", username);

        // Receive password from client

        recv(client_socket, password, sizeof(password), 0);

        password[strcspn(password, "\n")] = '\0';

        // printf("Received Password: %s\n", password);

        // Receive tokens from client

        recv(client_socket, &tokens, sizeof(tokens), 0);

        printf("No of tokens available: %d\n", tokens);

        FILE *file = fopen("user_data.txt", "a");

        if (file == NULL)

        {

            perror("Error opening file");

            exit(1);
        }

        // Write username, password, and tokens to the file

        // fprintf(file, "%s %s %d\n", username, password, tokens);

        fprintf(file, "%-15s %-15s \n", username, password);

        fclose(file);

        file = fopen("user_tokens.txt", "a");

        if (file == NULL)

        {

            perror("Error opening file");

            exit(1);
        }

        // Write username, password, and tokens to the file

        // fprintf(file, "%s %s %d\n", username, password, tokens);

        fprintf(file, "%-15s %-15s %-15d\n", username, password, tokens);

        fclose(file);

        while (1)

        { // Add an inner loop to keep serving the client until they exit

            int choice;

            int n = recv(client_socket, &choice, sizeof(int), 0);

            if (n <= 0)

            {

                break; // Exit the inner loop if there's an error
            }

            // Process the command using a switch statement

            if (tokens <= 0)

            {

                char acknowledgment[10];

                recv(client_socket, acknowledgment, sizeof(acknowledgment), 0);

                tokens = 5;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);
            }

            switch (choice)

            {

                char *videoFilePath;

            case 1:

                printf("Movie is selected.\n");

                videoFilePath = "movie.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 2:

                printf("Kapil shara show is selected.\n");

                videoFilePath = "Kapil.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 3:

                printf("F.R.I.E.N.D.S is selected.\n");

                videoFilePath = "friends.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 4:

                printf("India best dancer is selected.\n");

                videoFilePath = "dance.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 5:

                printf("NEWS is selected.\n");

                videoFilePath = "news.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 6:

                printf("Chotta bheem is selected.\n");

                videoFilePath = "bheem.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 7:

                printf("Tom & Jerry is selected.\n");

                videoFilePath = "tm.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 8:

                printf("CRICKET is selected.\n");

                videoFilePath = "cricket.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 9:

                printf("Animal planet is selected.\n");

                videoFilePath = "animal.mp4"; // Change this to your video file's path

                displayVideo(videoFilePath);

                tokens--;

                updateUserTokens(username, password, tokens);

                sendTokensToClient(client_socket, tokens);

                break;

            case 10:

                printf("Client requested to exit.\n");

                close(client_socket);

                break; // Exit the inner loop when the client requests to exit

            default:

                printf("Wrong station selected\n");

                break;
            }
        }

        printf("Client disconnected.\n");
    }

    close(server_socket);

    return 0;
}
