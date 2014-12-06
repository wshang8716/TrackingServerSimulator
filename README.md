OpenIGTLink Demo: Bi-directional Communication
==============================================

Overview
--------
This is an example program demonstrating how to write a "real-world"
application using the OpenIGTLink library. In particular, the example
program demonstrates how to handle bi-directional OpenIGTLink
communications, where OpenIGTLink messages are transfered in both ways
between the server and client.

The example program consists of following two programs:
- *Server* is the main demo program that receives requests from a client program
  and send dummy tracking data to the client.
- *Client* is a program to test the Server. It connects to the server program,
  request for dummy tracking data, and recieve them.


State Machine
-------------
In this simple example, three states for the server are defined: 
0. *StartUp:* The initial state after launching the server.
1. *Initialization:* The server initializes variables (and hardware, if available)
  after handshaking with the client.
2. *StandBy:* The server waits for a command from the client to triger tracking.
3. *Tracking:* The server repeats sending dummy tracking data to the client until
  it receives a stop command.

Message Exchange Scheme
-----------------------

### StartUp

|Server              |Message                           |Client                |
|--------------------|----------------------------------|----------------------|
|Start up the program|                                  |                      |
|                    |                                  |Start up the program  |
|                    |<< STRING("CMD", "INITIALIZATION")|Trigger Initialization|
|Enter Initialization|                                  |                      |



### Initialization

|Server                  |Message                             |Client              |
|------------------------|------------------------------------|--------------------|
|Send the current state  |>> STATUS("STATE", "INITIALIZATION")|                    |
|Initialize the variables|                                    |                    |
|Send the result         |>> STATUS("INITIALIZATION", OK)     |                    |
|Enter StandBy           |                                    |                    |


### StandBy

#### Enter StandBy state

|Server                  |Message                       |Client                |
|------------------------|------------------------------|----------------------|
|Send the current state  |>> STATUS("STATE", "STANDBY") |                      |
|Wait                    |                              |                      |


#### Start tracking

|Server                  |Message                       |Client                |
|------------------------|------------------------------|----------------------|
|                        |                              |"Start button" pressed|
|                        |<< STT_TDATA("TRACKING", fps) |Send a request        |
|Enter Tracking          |                              |                      |

#### Reset

|Server               |Message                          |Client                |
|---------------------|---------------------------------|----------------------|
|                     |                                 |"Reset button" pressed|
|                     |<< STRING("CMD", "INITALIZATION")|Send a request        |
|Enter Initialization |                                 |                      |


### Tracking

#### Enter Tracking state

|Server                  |Message                             |Client               |
|------------------------|------------------------------------|---------------------|
|Send the current state  |>> STATUS("STATE", "TRACKING")      |                     |
|Generate random data    |                                    |                     |
|Send randam data        |>> TDATA("TRACKING", pos)           |                     |
|Generate random data    |                                    |                     |
|Send randam data        |>> TDATA("TRACKING", pos)           |                     |
|  ...                   |   ...                              |                     |
|  ...                   |   ...                              |                     |


#### Stop tracking
|Server                  |Message                             |Client               |
|------------------------|------------------------------------|---------------------|
|                        |                                    |"Stop button" pressed|
|                        |<< STP_TDATA("TRACKING")            |Send a request       |
|Enter StandBy           |                                    |                     |

#### Reset

|Server               |Message                          |Client                |
|---------------------|---------------------------------|----------------------|
|                     |                                 |"Reset button" pressed|
|                     |<< STRING("CMD", "INITALIZATION")|Send a request        |
|Enter Initialization |                                 |                      |

