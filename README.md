OpenIGTLink Demo: Bi-directional Communication
==============================================

December 6, 2014

By Junichi Tokuda (tokuda@bwh.harvard.edu)


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

```
     +----------+
     | Start Up |
     +----------+
          |
          v
   +--------------+
   |Initialization|<-------+
   +--------------+        |
         |  ^              |
         v  |              |
     +----------+     +----------+
     | Stand by |<--->| Tracking |
     +----------+     +----------+
```



Message Exchange Scheme
-----------------------

### Message Notation
Following messages are used in the scheme:

- STRING(Name, Content)
  - Name: Message name
  - Content: ASCII string
  - See [OpenIGTLink Specification Page](http://openigtlink.org/protocols/v2_string.html)
- STATUS(Name, Code, SubCode, ErrorName, Message)
  - Name: Message name
  - Code: Status code
  - SubCode: Status sub-code
  - ErrorName: Name of error (<20 characters)
  - Message: String
  - See [OpenIGTLink Specification Page](http://openigtlink.org/protocols/v2_status.html)
- TDATA
  - TDATA(Name, [[TransName_0, Transform_0], …[TransName_N, Transform_N])
  - Name: Message name
  - TransName_i: Name of transform i
  - Transform_i: Transform i
  - See [OpenIGTLink Specification Page](http://openigtlink.org/protocols/v2_trackingdata.html)


### Common (All states)

####Request for the current status
|Server                  |Message                             |Client              |
|------------------------|------------------------------------|--------------------|
|                        |<< GET_STATUS("STATE")              |Send request        |
|Send the current state  |>> STATUS("STATE", OK, 0, State)    |                    |

'State' is a current status (i.e. STARTUP).


### StartUp (STARTUP)

|Server              |Message                           |Client                |
|--------------------|----------------------------------|----------------------|
|Start up the program|                                  |                      |
|                    |                                  |Start up the program  |
|                    |<< STRING("CMD", "INIT")          |Trigger Initialization|
|Enter Initialization|                                  |                      |



### Initialization (INIT)

|Server                  |Message                             |Client              |
|------------------------|------------------------------------|--------------------|
d|Send the current state  |>> STATUS("STATE", OK, 0, "INIT")   |                    |
|Initialize the variables|                                    |                    |


The server initialized the variables successfully:

|Server                  |Message                             |Client              |
|------------------------|------------------------------------|--------------------|
|Send the result         |>> STATUS("INIT", OK)               |                    |
|Enter StandBy           |                                    |                    |


The server could not initialize the variables:

|Server                  |Message                             |Client              |
|------------------------|------------------------------------|--------------------|
|Send the result         |>> STATUS("INIT", DNR)              |                    |



### StandBy
#### Enter StandBy state

|Server                  |Message                             |Client                |
|------------------------|------------------------------------|----------------------|
|Send the current state  |>> STATUS("STATE", OK, 0, "STANDBY")|                      |
|Wait                    |                                    |                      |


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
|                     |<< STRING("CMD", "INIT")         |Send a request        |
|Enter Initialization |                                 |                      |


### Tracking
#### Enter Tracking state
|Server                  |Message                              |Client               |
|------------------------|-------------------------------------|---------------------|
|Send the current state  |>> STATUS("STATE", OK, 0, "TRACKING")|                     |
|Generate random data    |                                     |                     |
|Send randam data        |>> TDATA("TRACKING", pos)            |                     |
|Generate random data    |                                     |                     |
|Send randam data        |>> TDATA("TRACKING", pos)            |                     |
|  ...                   |   ...                               |                     |
|  ...                   |   ...                               |                     |

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
|                     |<< STRING("CMD", "INIT")         |Send a request        |
|Enter Initialization |                                 |                      |




Implementation
--------------

