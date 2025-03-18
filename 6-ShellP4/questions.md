1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client determins the servers response is over when it recieves the RDSH_EOF_CHAR (0x04) character defined in rshlib.h. You can loop the recv function until this specific character is read in the biffer. This will combat against partial reads and wait until the response is finished. 

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell can define the beginning and end in several ways. The way we did it in this assignment was to define the end. But other ways could be by defining the length of the response in the response (at the beginning) so that the client knows how much data to expect. Challenges that could arise would be losing data and being stuck in recieve loops. 


3. Describe the general differences between stateful and stateless protocols.

A stateful protocol will keep track of info that pertains to the client. Like the number of requests. A ststeless protocol only recieves its info from the request made, so the server sends back the response and does not retain any info abbout the state of the client. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It does not need to hold connection. We use it when the speed of the response is more important than the stable connection.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

Sockets are what the OS provides so that we can send network communications. 