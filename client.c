#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>

void receiveTokensFromServer(int client_socket, int *tokens)

{

    if (recv(client_socket, tokens, sizeof(*tokens), 0) == -1)

    {

        perror("Error receiving tokens from server");
    }

    printf("No of tokens available: %d\n", *tokens);
}

int main()

{

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    char username[50];

    char password[50];

    int tokens = 5;

    char acknowledgment[10];

    acknowledgment[0] = 'a';

    if (client_socket == -1)

    {

        perror("Socket creation failed");

        exit(1);
    }

    // Connect to the server

    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(8080);

    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)

    {

        perror("Connection failed");

        exit(1);
    }

    printf("Connected to the server.\n");

    char station_list[1800]; // Buffer to store the list of stations received from the server

    // Receive and display the list of available and unavailable stations

    printf("\n-------------------------------------------\n");

    recv(client_socket, station_list, sizeof(station_list), 0);

    printf("%s\n", station_list);

    printf("10.Exit\n");

    printf("-------------------------------------------\n");

    // Send username to server

    printf("Enter Username: ");

    fgets(username, sizeof(username), stdin);

    send(client_socket, username, sizeof(username), 0);

    // Send password to server

    printf("Enter Password: ");

    fgets(password, sizeof(password), stdin);

    send(client_socket, password, sizeof(password), 0);

    if (strlen(username) == 0 || strlen(password) == 0)

    {

        printf("Enter username and password for tokens.\n");
    }

    else

    {

        send(client_socket, &tokens, sizeof(tokens), 0);
    }

    while (1)

    {

        printf("Choose the station you want: ");

        int choice;

        scanf("%d", &choice);

        if (tokens <= 0)

        {

            printf("No tokens available.\n");

            printf("Please answer the questions to gain more tokens.\n");

            char answer1[10], answer2[10], answer3[10];

            printf("Are you enjoying the videos...(yes/no)? ");

            scanf("%s", answer1);

            printf("Is kapil sharma show better than F.R.I.E.N.D.S...(yes/no)?");

            scanf("%s", answer2);

            printf("Did you watch chotta bheem and Tom & jerry...(yes/no)?");

            scanf("%s", answer3);

            send(client_socket, acknowledgment, strlen(acknowledgment), 0);
        }

        // Send the choice to the server

        send(client_socket, &choice, sizeof(int), 0);

        receiveTokensFromServer(client_socket, &tokens);

        if (choice == 10)

        {

            break;
        }
    }

    close(client_socket);

    return 0;
}
