+++
title = "The ingeScape network layer"
date = "2018-04-20T11:36:41+02:00"
tags = ["agents"]
categories = ["learn"]
banner = "img/banners/network_layer.jpg"
genre = "article"
+++

ingeScape enables to create distributed systems based on sets of connected software agents using the ingeScape library. Such architectures can be visualized and edited using the ingeScape editor.

This article presents standard and advanced capabilities brought by ingeScape to support a wide variety of network patterns, familiar in the domain of distributed systems and software buses.

The communication between ingeScape agents relies on a software bus, which is hidden behind our model-driven paradigm using *definitions* and *mappings*, explained in [the editor tour](/blog/editor_tour/).


### Fully decentralized, asynchronous and scalable

Most software buses and network systems use central brokers to make applications connect to the others. Some other solutions use hardcoded adresses and ports for these connections. The ingeScape community had the strong objective to provide a **fully decentralized** solution, and thus avoid these two practices, or at least not make them mandatory. 

That is why ingeScape environments are extremely flexible and insure that:

- The absence of an agent will not have another consequence than just making the agent's services temporarily unavailable,
- Subsets of agent can be formed quickly and easily to enable software development, testing and even mobility,
- Agent duplicates (i.e. having the same definition and possibly the same name) won't cause any trouble,
- Agents can come and go on the network without strong consequence on the other agents,
- Malfunctioning agents won't cause other trouble than possibly increasing the network trafic (and absolutely avoid saturation in other agents).

Agents on the same environment only have to share a common network port in order to create their connections to the others. Any ingeScape environment is a fully connected network between all its agents. This will fit with most on site local network configurations. And if heavier network constraints shall apply, ingeScape also supports more controlled network environments, involving security and connection brokers.

Modern systems are reactive or interactive, and heavily rely on multithreading. This is consistent with having **asynchronous** communications. The ingeScape API is fully asynchronous as well: writing an output value will result in its publication to the inputs of the interested agents. However, the publication is transparent to the developer. And so is the information reception, which triggers the *observe* callbacks for the implied agent inputs when the received data is actually available.

ingeScape has been designed to support hundreds of agents possibly involving hundreds of thousands of IOPs. This **scalability** is at the core of the ingeScape's network layer, together with the search for low latency, so that ingeScape environments can reach operational and industrial levels (without losing their simplicity).


### Patterns based on the ingeScape paradigm and library

To organize a given environment, ingeScape proposes a simple paradigms based on data flows between inputs and outputs of agents. This paradigm was designed to be as simple and straightforward as possible. However, this simplicity still enables to cover most of the possibilities offered by software bus solutions, sometimes without their pitfalls.

#### Publish/Subscribe

The most common pattern enabled by ingeScape is **Publish/Subscribe, also known as PUB/SUB**. This pattern enables one source to feed one or several destinations. The particularity is that the source has no actual knowledge of the destinations. This pattern is adapted to wide information distribution requirements.

This pattern is the preferred one because it shows both its relevance in many industrial situations, and its flexibility for modern information systems. Until now, the projects handled by the community have always been able to fit this pattern, even when involving third-party protocols and architectures.


#### Message-Oriented Middleware

[Message-Oriented Middleware](https://en.wikipedia.org/wiki/Message-oriented_middleware) (MoM) are the most common family among the software buses. They enable to exchange text or binary messages between components of distributed systems.

ingeScape addresses this possibility in two different manners:

- The first one is simply by using mappings from one output to several inputs, so that the inputs are fed with text, binary of numeric data. The mapping paradigm easily enables to create the links between all agents requiring to exchange messages.
- The second one is an actual MoM embedded in the ingeScape library. This MoM is used internally for metadata exchanges between agents. We made it available in the advanced ingeScape API so that developers can create topics (a.k.a. discussion channels) and send private messages of any kind.


#### Data Distribution Service

ingeScape does not support the [Data Distribution Service](https://en.wikipedia.org/wiki/Data_Distribution_Service) (DDS) as defined by the [Object Management Group](https://en.wikipedia.org/wiki/Object_Management_Group). However, it offers a simpler equivalent solution to the DDS DCPS layer and partially to the DLRL layer with the same spirit and objectives.

Based on the ingeScape paradigm and PUB/SUB pattern, it is very easy to conceive an agent (arm more) serving as data storage. This agent offers as many outputs as the variables it shall expose. Inputs can be added to this agent, to enable changing these variables. When it is done, this agent becomes a distributed shared memory space.

Agents interested in a variable just have to connect one of their inputs to the corresponding output. Agents wanting to modify a variable  may map one of their outputs to the corresponding input.

Finally, when an agent adds a new map from one of its inputs to another agent's output, this output is sent specifically to the input so that it is immediately updated with the latest value, so that later joiners or restarting agents do not miss at least the last value.


#### Enterprise Service Buse

An [Enterprise Service Bus](https://en.wikipedia.org/wiki/Enterprise_service_bus) (ESB) enables various types of software to offer and receive services over in network. The ingeScape paradigm is globally compliant with this approach. The *definition* models in ingeScape, that can be versioned if needed, are very similar to services descriptions. An agent can both provide and receive services to or from other agents, and redundancy is possible.

In addition to this, the advanced ingeScape API provides a services description mechanism, already used by the ingeScape editor and some existing agents, to adapt their behavior according to the presence or absence of a given service, attached or not to a specific agent. This description mechanism works in the same way as the [Zeroconf](https://en.wikipedia.org/wiki/Zero-configuration_networking), enabling each agent to publish sets of keys/values.


#### What about old good Client/Server ?

The ingeScape paradigm based on *definitions* and *mapping* does not cover the Request/Reply (a.k.a REQ/REP) pattern completely: with the ingeScape paradigm, it is difficult to have a server send its reply to a specific agent only by emitting on its outputs, because an agent has no knowledge of the mappings to its outputs.

However, the MoM features proposed by ingeScape, especially when using private messages for replies, cover REQ/REP perfectly. In addition, the ingeScape network layer implements the REQ/REP pattern natively, enabling to set it up if required by performance or security constraints.


#### Advanced patterns

The PUB/SUB pattern (and the REQ/REP pattern) is not fully convenient to cover advanced task delegation scenarios, for instance when a ventilator shall distribute tasks to an arbitrary and variable number of workers, which, in turn, send their results to a collector. Fortunately, the ingeScape network layer, combined with some expertise, can handle these situations.

For other patterns engaging reliability, redundancy, synchronicity, etc., ad-hoc solutions are always possible, often without breaking the simplicity of the ingeScape paradigm. The paradigm exposed by ingeScape relies on a network layer that already covers many difficulties related to network communications and can be extended to face real-world challenges, at least as efficiently as many other software buses.


### Other network features

ingeScape aims at making distributed architectures as easy as possible. But it does not neglect or ignore common and advanced challenges faced by heterogeneous systems relying on network communications.

In addition to the advanced patterns described before, the ingeScape network layer covers the following :

- Data serialization, even for very large data streams,
- Low-level protocols other than TCP/IP and including UDP, Multicast, IPC, TIPC, shared memory, etc.
- Time management (latency, synchronicity, timeouts, etc.)
- Multithreading and threads synchronisation (with a sane state of mind, avoiding the multiplication of mutexes and other sources of deadlocks)
- Authentification and encryption

