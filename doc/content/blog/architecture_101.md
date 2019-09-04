+++
title = "First steps through the design and architecture of an ingeScape agent"
date = "2018-04-20T11:36:41+02:00"
tags = ["architecture"]
categories = ["learn"]
banner = "img/banners/architecture.png"
genre = "article"
+++

This article shows some best practices that have been established when developing ingeScape agents from scratch. These practices will insure that the agent can evolve quickly in time and also includes reusable ingeScape-independent software that could be useful in many other scenarios.

**Should you need to adapt existing software into an ingeScape agent, some relevant strategies are presented in the last section of this article.**

Before going further we advise that you read at least:

- [Your first ingeScape agent](/blog/your_first_agent)
- [The editor tour](/blog/editor_tour)
- [Your first complete agent](/blog/first_full_agent)

## An agent seen from the outside
With ingeScape, agents are parts of global distributed software platforms. They are assembled so that their local behaviours, combined with others, can create new, meaningful, value-adding behaviours and services. Agents can thus be considered as software components that expose a programming interface to configure and use them. This programming interface takes complementary forms.

### Command line parameters and console commands
Agents are independent pieces of software. Many agents, especially non-graphic ones, are executed using the command line, just like any other software. The command line offers parameters to configure the execution of an agent. That is all the things that will not change during the execution, wether this execution is short or lasts months (agents can run as daemons or services in operating systems).

Here is a minimal list of command line parameters to consider when creating a new agent :

- *help* : certainly the most important one, in charge of listing all the other parameters with their default values and special instructions.
- *network device* and *port* : these are mandatory for the agent to start on the network. You need to give your agent a way to define them and the simplest one is passing them through command line arguments. Note that other strategies are possible such as putting these parameters in a configuration file, enabling the agent to introspect the compatible network devices on your computer, or even scan ports to detect the presence of other agents.
- *agent name* : sometimes, it makes sense to change the name of an agent to make it more meaningful or to efficiently deal with multiple instances of an agent on the same platform (even if ingeScape manages clones properly).
- *verbose* and log file path : depending on the execution context, watching logs in the console or collecting them in a log file can be a lifesaver. The ingeScape API provides everything needed to control that and exposing these possibilities as command lien parameters is always useful.
- *definition and mapping path* : these are not mandatory as agents can live a long and happy life with a hardcoded definition and by having their mapping being controlled by an ingeScape editor. However, using external definition or mapping enable to manage different versions of them, to establish contracts between developers, to effortlessly have various definitions for a single implementation and to share these contracts even before an agent is actually developed.
- *noninteractiveloop* : this one is explained in [your first full agent](/blog/first_full_agent) example and is about having an agent running interactively inside a console or as a daemon or a background program.

When running in a console, an agent can receive commands from the standard input. Though old-fashioned, it may be an extremely efficient way to test or configure agents during their execution, and to awake the power-user inside you.

### Inputs, Outputs and Parameters, a.k.a. the IOPs
The [IOPs](/blog/your_first_agent#inputs-outputs-and-parameters-the-iops) are the most visual and efficient parts of the ingeScape model to control an agent and make it useful. The IOPs enable the design of a whole dataflow network inside a distributed software platform. Dataflow is a Turing-complete extremely powerful model to orchestrate pieces of software.

Here is the example definition of our *igsMail* agent offering email services to any other agent running on the same platform:
{{< highlight json "linenos=table,linenostart=1" >}}
{
    "definition": {
        "name": "igsMail",
        "description": "NO_DESCRIPTION",
        "version": "1.0",
        "parameters": [
            {
                "name": "login",
                "type": "STRING",
                "value": "test@ingenuity.io"
            },
            {
                "name": "password",
                "type": "STRING",
                "value": "my_password"
            },
            {
                "name": "server",
                "type": "STRING",
                "value": "smtp://smtp.ingenuity.io"
            }
        ],
        "inputs": [
            {
                "name": "from",
                "type": "STRING",
                "value": ""
            },
            {
                "name": "to",
                "type": "STRING",
                "value": ""
            },
            {
                "name": "cc",
                "type": "STRING",
                "value": ""
            },
            {
                "name": "subject",
                "type": "STRING",
                "value": ""
            },
            {
                "name": "body",
                "type": "STRING",
                "value": ""
            },
            {
                "name": "sendMessage",
                "type": "IMPULSION",
                "value": ""
            }
        ],
        "outputs": [
            {
                "name": "sendStatus",
                "type": "STRING",
                "value": ""
            }
        ]
    }
}
{{< / highlight >}}

<br>
Inputs are the way to feed an agent. There are many different approaches in design an agent’s inputs: they can be independent or complementary, atomic and information-rich (especially when using the DATA type in ingeScape), data-centric or event-centric, etc. When designing an agent, think of the inputs are the ways to activate your agent and provide it with information in relation with its internal objectives and behaviour. Some agents may not have inputs. This is the case for agents encapsulating user-input devices such as an eye tracker, a spatial gestures device or even a simple keyboard.

Symmetrically, an agent’s outputs are what makes your agent useful to the rest of the environment by providing events, information or reports associated with the service your agent manages. It is about what you want your agent to make available to the rest of your platform. Some agents may not have any output. This is the case for agents in charge of reaching the outside world, such an agent capable of sending emails or dropping files (received as data on their inputs) to a server, etc.

The correlation between the inputs and the outputs defines the dynamics of an agent. With the dataflow model, an agent does not know where information on its inputs come from and where information to its outputs go to. This loose relation with other agents is exactly what we are looking for in evolutive, distributed, flexible distributed environments. That is why it is extremely important never to make any assumption about who will feed your agent’s inputs and who will use its outputs.

In addition, parameters are here to enable dynamic configuration of an agent by using the dataflow model. At the moment, only an ingeScape editor (or an ingeScape sorcerer) is capable of writing to an agent’s parameters dynamically. It is also possible to configure parameters values when loading a definition in an agent. Think of the parameters as ways of rarely change the internal behaviour of your agent. This may be useful when starting a whole platform or when a major event occurs in your platform architecture or in your environment but should not happen as often as writing your inputs. If a parameter has to change frequently, it certainly is that this parameter should rather be an input or that there is a design flaw somewhere. A good example of parameters are the server address, the login and the password for an agent in charge of sending emails : these parameters need to change only when the server changes or when you are setting up a new platform in a new environment.

### Calls
Calls are here because sometimes, dataflow is not the fastest way to achieve communication or trigger behaviours. With calls, it is easy to trigger a mechanism on any agent by just knowing its name and to provide necessary information to do so. Calls are just like functions or methods calls in programming languages. By opposition to IOPs, they require to know the ecosystem you evolve in, in order to use them. Their use shall thus be limited to very specific or exceptional situations where knowing the agents emitting and/or receiving the call makes sense or is necessary. For other situations, calls can easily be replaced by inputs of type IGS_DATA fed with all the information pieces that would be given as arguments to the call.

Here is the part of a definition actually creating a call:
{{< highlight json "linenos=table,linenostart=1" >}}
        "calls": [
            {
                "name": "sendMail",
                "arguments": [
                    {
                        "name": "from",
                        "type": "STRING"
                    },
                    {
                        "name": "to",
                        "type": "STRING"
                    },
                    {
                        "name": "cc",
                        "type": "STRING"
                    },
                    {
                        "name": "subject",
                        "type": "STRING"
                    },
                    {
                        "name": "body",
                        "type": "STRING"
                    }
                ]
            }
        ]
{{< / highlight >}}

Here is how to execute this call:
{{< highlight c "linenos=table,linenostart=1" >}}
    igs_callArgument_t *list = NULL;
    igs_addStringToArgumentsList(&list, "\"Test\" <test@ingescape.io>"); //from
    igs_addStringToArgumentsList(&list, "\"contact 1\" <contact1@ingescape.com>"); //to
    igs_addStringToArgumentsList(&list, "\"contact 2\" <contact2@ingescape.com>,\"Contact 3\" <contact3@ingescape.com>"); //cc
    igs_addStringToArgumentsList(&list, "Testing email sent using a call"); //subject
    igs_addStringToArgumentsList(&list, "This is a test\nto check that sending calls\nis super simple."); //body
    igs_sendCall("igsMail", "sendMail", list);
{{< / highlight >}}

And here is the callback for this call in our *igsMail* agent:
{{< highlight c "linenos=table,linenostart=1" >}}
void sendMailCallCB(const char *senderAgentName, const char *senderAgentUUID,
                     const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                     void* myData){
    // Avoid unused parameter warning
    (void)senderAgentName;
    (void)senderAgentUUID;
    (void)callName;
    (void)myData;
    
    if (nbArgs == 5){
        char *error = NULL;
        sendMail(firstArgument->c, //from
                 firstArgument->next->c, //to
                 firstArgument->next->next->c, //cc
                 firstArgument->next->next->next->c, //subject
                 firstArgument->next->next->next->next->c, //body
                 NULL, 0,
                 (const char **)&error);
        if (error){
            igs_error("curl_easy_perform() failed: %s", error);
            igs_writeOutputAsString("sendStatus", error);
        }else{
            igs_info("Mail sent with success");
            igs_writeOutputAsString("sendStatus", "OK");
        }
    }
}

//...

//registered this way in the main() function
igs_initCall("sendMail", sendMailCallCB, NULL);

{{< / highlight >}}

<br>
And here is the code to declare a call directly in your application instead of using a JSON definition file:
{{< highlight c "linenos=table,linenostart=1" >}}
    igs_initCall("sendMail", sendMailCallCB, NULL);
    igs_addArgumentToCall("sendMail", "from", IGS_STRING_T);
    igs_addArgumentToCall("sendMail", "to", IGS_STRING_T);
    igs_addArgumentToCall("sendMail", "cc", IGS_STRING_T);
    igs_addArgumentToCall("sendMail", "subject", IGS_STRING_T);
    igs_addArgumentToCall("sendMail", "body", IGS_STRING_T);
{{< / highlight >}}

<br>
Agents may have an advantage in exposing calls when they offer specific discrete services. The example of an agent in charge of sending emails is relevant : sending an email just requires providing the email parameters (from, to cc, subject, etc.). Dataflow is not particularly helpful for this and the email agent can be addressed by its name without any confusion. However, in the email agent we designed, both calls and IOPs are usable in parallel to give users the choice of their strategy. 

Another example is our *igsSocket* agent : this agent creates data inputs and ouputs for classic TCP sockets. Dataflow is very convenient both for reading and writing to the socket. However, creating and removing a socket is achieved by using calls which contain all the necessary parameters (socket name, address and port, plus a parameter to determine if the socket is server or client). The combination between calls for dynamic creation/removal of sockets and IOPs for reading and sending data is very flexible and appreciated.

Here is the code for our declared calls handling sockets creation and removal:
{{< highlight c "linenos=table,linenostart=1" >}}
    igs_initCall("ADD_SOCKET", addSocketCall, NULL);
    igs_addArgumentToCall("ADD_SOCKET", "name", IGS_STRING_T);
    igs_addArgumentToCall("ADD_SOCKET", "address", IGS_STRING_T);
    igs_addArgumentToCall("ADD_SOCKET", "port", IGS_INTEGER_T);
    igs_addArgumentToCall("ADD_SOCKET", "isServer", IGS_BOOL_T);
    
    igs_initCall("REMOVE_SOCKET", removeSocketCall, NULL);
    igs_addArgumentToCall("REMOVE_SOCKET", "name", IGS_STRING_T);
{{< / highlight >}}

### Software bus and topics/channels
ingeScape has been inspired by famous software buses that historically were message-oriented. To help distribute these messages to multiple entities at the same time from a single source, they often introduced the concept of channel or topic : every software connected to a channel/topic would receive any message published by another software on this topic, adding to this the possibility to send direct messages from one software to another, generally using names. This is exactly the same metaphor as the good old IRC protocol with people chatting on channels and exchanging private messages.

ingeScape provides an API to create such a bus and to publish or subscribe to channels. ingeScape also provides monitoring tools to list channels and watch what is happening on them. This is done is a very performing way and can support massive amounts of data, just like IOPs dataflow. But this is the old way to do it : no dynamic control of the flow, no data type or model-based description for exchanged data, no visual integration in an editor, no archiving of private messages, etc. That is why we recommend to use dataflow or calls to create modern distributed systems, except of course if you want to implement an enterprise-grade chat solution in a few minutes.

Here is an example of callback implementation and registration to use the bus and receive private *WHISPER* messages:
{{< highlight c "linenos=table,linenostart=1" >}}
void incomingMessageOnBus (const char *event, const char *peer, const char *name,
                           const char *address, const char *channel,
                           zhash_t *headers, zmsg_t *msg, void *myData){
    // Avoid unused parameter warning
    (void)peer;
    (void)name;
    (void)address;
    (void)channel;
    (void)headers;
    (void)myData;
    
    if(streq (event, "WHISPER")){
        //this is a private message
        char *message = zmsg_popstr (msg);
        if (strcmp(message, "SEND_MAIL") == 0){
            const char *error = NULL;
            char *from = zmsg_popstr (msg);
            char *to = zmsg_popstr (msg);
            char *cc = zmsg_popstr (msg);
            char *subject = zmsg_popstr (msg);
            char *body = zmsg_popstr (msg);
            sendMail(from, to, cc, subject, body, NULL, 0, &error);
            free(from);
            free(to);
            free(cc);
            free(subject);
            free(body);
        }
        free(message);
    }
}

//...

igs_observeBus(incomingMessageOnBus, NULL);
{{< / highlight >}}

<br>
And here is the code to send a private message to *igsMail*:
{{< highlight c "linenos=table,linenostart=1" >}}
    //here we use the underlying ZeroMQ zmsg structure to ease the creation of a multipart message
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, "SEND_MAIL");
    zmsg_addstr(msg, "\"Test\" <test@ingenuity.io>"); //from
    zmsg_addstr(msg, "\"contact 1\" <contact1@ingescape.com>"); //to
    zmsg_addstr(msg, "\"contact 2\" <contact2@ingescape.com>,\"Contact 3\" <contact3@ingescape.com>"); //cc
    zmsg_addstr(msg, "Testing email sent using the bus"); //subject
    zmsg_addstr(msg, "This is a test\nto check that sending bus messages\nis super simple."); //body
    igs_busSendZMQMsgToAgent("igsMail", &msg);
{{< / highlight >}}



## Reusability inside an agent
An agent provides a service… just like a software library does. And some agents perfectly make sense by just wrapping a powerful software library and making this library available to all other agents inside a platform. This is basically what we did for our *igsMail* agent, simply wrapping the CURL library and formatting messages as expected by the SMTP protocol.

However, whatever the service the agent is providing and wether it is based on a single or multiple libraries, it is always a good idea to have a « high-level library » approach. This means that your service should be available (if we take the example of the C programming language) as a high-level header with all the functions, types and variables useful for the service. All the other code should be well encapsulated behind this header, and structured in a number of files or modules fitting the complexity of the implementation.

The main advantage of this approach is that the encapsulation behind a high-level header makes things simple for the rest of the code in the agent (see next section) and makes your module reusable independently from an ingeScape context : should you need to integrate your module inside another application or use it with another network library, you could use it in almost the same way.

Once the high-level header has been designed and developed to match with the defined model for the agent including IOPs and calls, one needs to finally write the code for the agent, i.e. code the reactions between inputs, outputs and calls, including the exposed parameters...

## Coding an agent : it’s all about the glue…
We already covered how to code command line parameters and console commands in our [Your First complete agent](/blog/first_full_agent). Creating IOPs and observing them is described in [Your first ingeScape agent](/blog/your_first_agent).

Basically, coding an agent is mixing the following things in your callbacks code (i.e. observe callbacks for IOPs, calls, etc.):

- Information received on the inputs,
- Received calls and attached information,
- Possible use of the agent parameters values,
- Resulting behaviors involving function calls and algorithms in your internal code and writing information to the agent outputs.

If the high-level headers encapsulating the services offered by your agent are well designed, all this glue code should perfectly fit inside your main application file and represent only a few dozens of lines of code.

As a reminder, here is the callback code for [Your first ingeScape agent](/blog/your_first_agent) that reads the input and publish its value multiplied by two on its output:
{{< highlight c "linenos=table,linenostart=1" >}}
void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData){
    /*
     This callback is fed with all the information necessary:
     - the IOP type : input, output or parameter
     - the IOP name
     - the IOP value type (impulsion, boolean, etc.)
     - a pointer to the value
     - the actual memory size of the value
     - optional data passed from the call to observeInput
     */
    int r = igs_readInputAsInt(name);
    printf("%s changed to %d\n", name, r);
    igs_writeOutputAsInt("output1", 2*r); //publishing the received value multiplied by 2
}
{{< / highlight >}}


## Integrating with legacy code and industrial frameworks
A **real-world example** of ingeScape integration into complex existing software is presented in [this article](/blog/vlc).

Sometimes, an agent may be based on a legacy application with its own architecture or an application based on an industrial framework such as Qt, Microsoft .Net or Apple Cocoa. In these situations, the application initialization process and the internal architecture are constrained. However places to integrate the ingeScape code and make this legacy or industrial application an agent are pretty much the same, independently from the underlying architecture.

The ingeScape library creates its own set of threads and does not need to rely on the execution control of the main application. We designed the ingeScape library so that it can be called from any place inside an existing application code. These clean separation and easy access offer a total integration flexibility. Integrating ingeScape code inside an industrial or legacy application can be seen simply as « surgically » minimally adding the ingeScape callbacks and function calls where it suits best inside the existing or constrained code.

The ingeScape library is developed in C. This enables portability on any operating system and wrapping inside any programming language. In addition to C++ and Objective-C that can include the ingeScape C API without any cost, ingeScape has already been wrapped in C# for Microsoft environments, in Python to quickly create agents by using the amazing work of the Python community, and in Javascript/QML for a seamless integration in Qt Creator. Java will be coming shortly.

Independently from the programming language, here are the major steps for transforming an industrial or legacy software into a proper and well designed ingeScape agent:

- The ingeScape library first requires some initialization in order to configure the agent name, its definition and other relevant parameters (logging, links with command line parameters, etc.). This is generally achieved in the application’s main class or function or in a dedicated software class that is created for the occasion. In any case, this represents less than one hundred lines of codes.
- If the agent requires to declare inputs, these inputs generally need to be observed and thus to be attached to callbacks. The callbacks can be developed in your application’s preferred language by relying on the language wrapping that has been achieved (in C#, Java, etc.). The callbacks may then trigger internal code in your application to react to the received inputs and then write ingeScape outputs or call calls if relevant.
- Finally, some internal behaviors in your application or some user interactions may also require to write ingeScape outputs or to send calls. Generally, this is achieved by simply adding the corresponding instructions inside the existing application functions, where the information to be used or sent is the most accessible.

In our experience, transforming even a very large or very old application into an ingeScape agent only requires a few hundreds lines of code to be added in strategic places, without requiring to make any change in the existing code architecture. Due to ingeScape independent threads, there no influence in the existing execution control mechanisms, except sometimes for the necessity to call some code from the ingeScape callbacks inside one of the application’s threads or queues, using provided frameworks functions to do so. This is practically the same as having a dedicated UI thread and other threads for heavy algorithms, except that ingeScape does not require complex synchronisation mechanisms.



