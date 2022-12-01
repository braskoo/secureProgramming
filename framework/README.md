# Assignment 1A

The code provided implements the basic messaging functions for a chat application consisting of two programs: a client
program and a server program.

The server accepts a max number of clients (16), in a way that each client connects to a worker via a socket and the clients connect to the server via a pipeline.


## Remarks about the code

Added files: 

    * types.h : generic header file including different definitions both for api.c, client.c and ui.c (used to facilitate use of include). This files includes the union CODE, and the structs api_state, api_message, ui_state and client_state
        - api_msg: contiguous block in heap memory. Consists of a union CODE used for either response codes or request codes, a message size for security purposes and a message. Since this is all stored contiguously, we can send the message directly with only the api_msg pointer and the msg size
    * database.db : basic database with only one table for messages, saving both the public and private messages. Since we didn't implement user autentication for this week, we identify the users by their fd.

Previous files:
    * ui.c : implements the user interface, it includes the functions
        - ui_state_free : frees the memory previously allocated by user.c (TO DO)
        - ui_state_init : asserts the ui state
        - ui_state_fill : fills in a ui state using a string (line input)
        - ui_command_parse : parses the command stored in its parameter into an enum code used for API messages

    * util.c : provides auxiliary functions to facilitate communication
        - lookup_host_ipv4
        - parse_port

    * client.c : client program, includes the functions:
        - handle_server_request: receives a message from the server, executes the request and frees the memory (TO DO)
        - handle_incoming: registers and processes IO events
        - client_state_init
        - client_state_free
        - main

    * worker.c : acts as an intermediary between the client and the server. The client connects to the server via a socket, while the worker connects to the server via a a socket pair.
        - handle_s2w_notification: handles notifications from the server to the worker (TO DO)
        - execute_request : executes a request from the client, right now only the basic implementation for messages is done, a basic skeleton for the others is provided
        - handle_client_request
        - handle_s2w_read
        - handle_incoming
        - worker_state_init
        - worker_state_free

    * server.c, includes structs server_child_state and server_state, and the functions for handling the server (create_server_socket,child_add, children_check, close_server_handles, etc). These functions were kept as provided.

    * api.c : describes the procedure used by both the server and client to send and recieve, as well as compose api messages
        - api_debug_msg : prints the details of an api_message sent or recieved, used for debugging
        - api_recv : allocates a new api_msg in heap memory and fills it with the recieved data
        - api_msg_compose : allocates a new api_msg in heap memory and fills it with the parameter data
        - api_state_free : frees any data affiliated with the api_state. This function is unneccesary for now since the api_state only contains primitive types
        - api_state_init : initialises api state
        - api_send : sends the data in api_msg to the fd specified in api_state

## Installing and running the project