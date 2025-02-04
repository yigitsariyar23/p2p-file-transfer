# Peer-to-Peer File Sharing System

### Introduction

This project demonstrates a simplified peer-to-peer (P2P) file sharing
system, reminiscent of how torrent-like file distribution can be
handled. It consists of two main components:

1.  ****Server****:

    - Manages metadata about shared files (i.e., which files are shared
      and by which peers).
    - Responds to client requests to list files, or to locate a
      particular file's owner.

2.  ****Client****:

    - Connects to the server, announces files it wants to share, and
      requests file information.
    - Runs a small "peer server" thread that allows other clients to
      directly request and download files over TCP.

The P2P file transfer occurs ****directly**** between clients once the
server supplies the necessary information (IP and port of the file's
owner).

### Features / What the Service Does

1.  ****Share (Announce) a File:****

    - The Client notifies the Server that it has a particular file
      available.
    - The Server stores the file metadata (filename, IP of the sharer,
      and port where it can be requested).

2.  ****List Shared Files:****

    - A Client can request from the Server the list of files that are
      currently known/shared by any users.
    - The Server responds with all filenames and associated peer
      addresses.

3.  ****Download File (REQUEST command):****

    - A Client asks the Server for the location (IP, port) of the peer
      owning a specific file.
    - The Server returns "FOUND \<ip\> \<port\>" or "NOT_FOUND".
    - If found, the Client uses that IP and port to connect
      ****peer-to-peer**** and request the file.
    - The peer sends the file directly to the requesting client over
      TCP.

The server does ****not store or relay file contents **** - only
metadata. File data is transferred P2P, making the system more scalable.

### System Overview & Implementation

- ****Server****:

    - Listens on a TCP port for incoming client connections.

  - Each client connection is handled in a separate thread.

  - Uses a shared data structure (an array) to store file information
    (**file_name**, **owner_ip**, **owner_port**).

  - Provides commands:

    - **REGISTER \<username\>**: Logs the user connection (for
      demonstration purposes).
    - **ANNOUNCE \<filename\> \<port\>**: Stores metadata for the file.
    - **LIST_FILES**: Sends back the list of all available files (with
      IP and port).
    - **REQUEST \<filename\>**: Returns **FOUND \<ip\> \<port\>** if the
      file is found in the metadata, otherwise **NOT_FOUND**.

  - Uses a mutex to protect the shared list of files (concurrent thread
    safety).

- ****Client****:

  - Runs in two parts:

    - ****Peer Server Thread****: Listens for inbound file download
      requests from other clients.
    - ****Server Communication****: Connects to the main server, sends
      commands (**ANNOUNCE**, **LIST_FILES**, **REQUEST**, etc.).

  - Once it receives the location of a file's owner, it initiates a
    peer-to-peer TCP connection to request the file directly.

  - It also provides a simple command line interface (CLI) for the user.

#### Server Flow

1.  ****Start****: The server listens on a specified port (supplied as
    command-line argument).

2.  ****Accept Loop****: For each incoming TCP connection:

    - Spawn a thread to handle the client.

3.  ****Thread Function**** (**handle_client()**):

    - Reads the protocol version from the client. Only **PROTO_VERSION
      1.0** is accepted.
    - Waits for subsequent commands (REGISTER, ANNOUNCE, LIST_FILES,
      REQUEST, etc.).
    - Updates or reads the global **files** array as needed (protected
      by a mutex).

4.  ****Cleanup****: On client disconnect, remove that peer's files from
    the list.

####  Client Flow

1.  ****Startup****:

    - The client is launched as:

      **./client \<username\> \<server_ip\> \<server_port\>
      \<my_peer_port\>**

    - A dedicated peer server thread is started on **\<my_peer_port\>**,
      so others can download from us.

2.  ****Server Connection****:

    - The client connects to the main server at
      **\<server_ip\>:\<server_port\>**.
    - Sends **PROTO_VERSION 1.0**. Exits if the server replies with
      **VERSION_FAIL**.
    - Sends **REGISTER \<username\>** to register this user session with
      the server.

3.  ****CLI Loop****:

    - The client waits for user commands:

      - **ANNOUNCE \<file\>**: Tells the server it has **\<file\>**.
      - **LIST_FILES**: Requests the full list of known files.
      - **REQUEST \<file\>**: Asks the server for the location of
        **\<file\>**. If found, downloads it P2P from the specified
        IP:port.
      - **EXIT**: Terminates the client.

4.  ****Peer Download****:

    - If the user typed **REQUEST \<file\>** and the server responded
      with **FOUND \<ip\> \<port\>**, the client opens a TCP connection
      to **\<ip\>:\<port\>** and sends the filename. The peer then
      streams the file contents back.

### How to Compile

- ****Compile the server****:

    - **gcc -pthread -o server server.c**

- ****Compile the client****:

    - **gcc -pthread -o client client.c**

### How to Run

- ##### Running the Server

    1.  Start the server on a specified port (e.g., 8080):  **./server 8080**

    2.  The server listens for new TCP connections on port 8080.

    3.  ****Log Output****: Any important server-side events are logged to **server.log**.

- ##### Running the Client

    1.  Launch the client with:

        **./client \<username\> \<server_ip\> \<server_port\>
    \<my_peer_port\>**

        For example:

        **./client **yigido** 127.0.0.1 8080 9001**

        - **yigido**: the username
        - **127.0.0.1**: address of the server if running locally
        - **8080**: must match the server\'s listening port
        - **9001**: the port on which this client will accept P2P requests
          from other clients

    2.  ****Client Commands****:

        - ******ANNOUNCE \<filename\>******  
          Tells the server we have **\<filename\>** to share.
        - ******LIST_FILES******  
          Retrieves the list of all shared files from the server.
        - ******REQUEST \<filename\>******  
          Asks the server who has **\<filename\>**. If found, automatically
          downloads it from that peer.
        - ******EXIT******  
          Closes the client program.

## References / Sources

-  [The Definitive Guide to Linux Network
  Programming.pdf](https://kalfaoglu.com/ceng421/The%20Definitive%20Guide%20to%20Linux%20Network%20Programming.pdf)
- [Data Communications and Networking By Behrouz
  A.Forouzan](https://kalfaoglu.com/ceng421/Data%20Communications%20and%20Networking%20By%20Behrouz%20A.Forouzan.pdf)
