---
title: "ingeScape editor, take the tour !"
date: 2018-04-23T15:35:00+02:00
draft: false
---

This article presents the high-level features of the ingeScape editor. We do not go into details here but you will have a good overview of this application after your read.

Whereas the ingeScape library is dedicated to software developers, the ingeScape editor has been designed for all the other actors who require specific visualizations or tools. The editor leverages the benefits of a graphical application to accelerate or extend tasks that can be achieved through coding.

Systems built on ingeScape can contain a large number of agents and communication flows. The ingeScape editor introduces features to monitor and configure them dynamically. The editor exposes the agents ***definitions*** and the ***mappings*** between agents, which are the key ingeScape concepts, detailed in the "[Your first ingeScape agent](./blog/your_first_agent)" example. Other concepts join them to provide new services to the platform users.

The current version of the editor offers the following services:

- Supervision of agents and machines
- Visualization and edition of mappings between agents
- Scenarios edition and execution
- Platform data record and replay

The ingeScape editor presents three major areas, one of them being hidden when the editor starts:

1. The tabs gives access to the list of agents, machines and actions.
+ The main central area presents the agents involved in the current mapping with all the actual mapping links between agents.
+ The timeline, at the bottom, stores all the actions scheduled or executed in the current scenario.

![editor composition](/snapshots/main_composition.png)


We will now go through these services and provide more details.

## Supervision of agents and machines
When executed on a given system, i.e. on a given network with a given network port, the editor collects and shows the list of all the active agents and offers to call various types of commands on them (stop, mute, freeze, etc.). The editor gives access to the details of a definition in a dedicated view.

![supervision](/snapshots/supervision.png)

The editor can also import agent definitions before an agent is actually available in the system. In the same way, agents that are not executed, but were before, are still listed as inactive agents. The editor is able to start inactive agents if their hosting computer is switched on and present on the network.

Agent definitions can be exported and imported with the editor using a simple JSON format. The ingeScape library supports the loading and dumping of these JSON files so that the developers do not have to always hardcode an agent definition but use an external resource file instead.

Here is an example of a simple agent definition:

{{< highlight json "linenos=table,linenostart=1" >}}
{
    "definition": {

        "name": "myAgent",
        "description": "myAgent is able to ...",
        "version": "0.1",
        "parameters": [{
                       "name": "param1",
                       "type": "INTEGER",
                       "value": -1
                       }],
        "inputs": [{
                   "name": "myImpulsion",
                   "type": "IMPULSION",
                   "value": ""
                   },
                   {
                   "name": "myInt",
                   "type": "INTEGER",
                   "value": 30
                   },
                   {
                   "name": "myBool",
                   "type": "BOOL",
                   "value": "false"
                   },
                   {
                   "name": "myDouble",
                   "type": "DOUBLE",
                   "value": 15.5
                   },
                   {
                   "name": "myString",
                   "type": "STRING",
                   "value": "string example"
                   },
                   {
                   "name": "myData",
                   "type": "DATA",
                   "value": ""
                   }],
        "outputs": [{
                    "name": "value1",
                    "type": "DOUBLE",
                    "value": 3.14
                    }]
    }
}
{{< / highlight >}}

Just like for definitions, the editor provides a list of active machines in the observed system. Such machines, that can operate on Linux, Windows or macOS, embed a specific software agent giving them capabilities regarding ingeScape and hosted agents. This software can be run as a daemon or system tray program.

![hosts](/snapshots/hosts.png)


## Visualization and edition of mappings between agents
At startup, the ingeScape editor connects a system and draws its global mapping, i.e. all the agents composing the system and all the links between the inputs and outputs. This global mapping is presented as a set of boxes representing the agents, and the "wires" connecting them.

![mapping](/snapshots/mapping.png)

The mapping view is a zoomable infinite workspace where users can draw and organize their agents and associated connections. When the editor is connected to a system, mapping links can be added or removed dynamically and the systems updates itself automatically to implement the new mapping.

When agents exchange data, i.e. when an agent's output is written, the existing mappings from this output to inputs of other agents are highlighted, providing a feedback of the circulating data. The editor is also able to show the actual values that are exchanged, whatever their format is (numbers, text, etc.).

Just like definitions, mappings can be hardcoded by developers or managed through JSON files.

Here is an example of a simple agent mapping:
{{< highlight json "linenos=table,linenostart=1" >}}
{
    "mapping": {
        "name": "myOtherAgentMapping",
        "description": "This mapping is the one for myOtherAgent",
        "version": "0.1",
        "mapping_out": [{
                        "input_name": "myImpulsion",
                        "agent_name": "myAgent",
                        "output_name": "myImpulsion"
                        },
                        {
                        "input_name": "myInt",
                        "agent_name": "myAgent",
                        "output_name": "myInt"
                        },
                        {
                        "input_name": "myBool",
                        "agent_name": "myAgent",
                        "output_name": "myBool"
                        },
                        {
                        "input_name": "myDouble",
                        "agent_name": "myAgent",
                        "output_name": "myDouble"
                        },
                        {
                        "input_name": "myString",
                        "agent_name": "myAgent",
                        "output_name": "myString"
                        },
                        {
                        "input_name": "myData",
                        "agent_name": "myAgent",
                        "output_name": "myData"
                        }
                        ]
    }
}
{{< / highlight >}}

The example above shows a strict one to one output to input mapping from the outputs of the "myAgent" agent to another agent that would be called "myOtherAgent".

Any combination of mappings is allowed. An output can be connected to as many inputs as you like. An input can be fed by as many outputs as you like. You need to be careful though to avoir loops in your architecture and concurrency on a given input, if fed by several outputs at once. The ingeScape library has some advanced features enabling to deal with the most complex architectures in truly distributed and highly evolutive systems, based on state of the art network patterns.


## Scenarios edition and execution
The previous paragraphs present how the ingeScape editor gives facilities for the core ingeScpae concepts. Above this, the editor brings concepts of its own for non-developers wanting to create and execute behaviors on a given system.

The editor introduces the concept of ***action*** to be executed in your system. Actions are composed of *effects*, *conditions* and advanced parameters:

- *Effects* are the actual results of an action in your system. They can be start/stop of an agent, creation or removal of a mapping, value writing on any agent's IOP, etc.
- *Conditions* are optional checks regarding an action that will set if the effects can be applied or not. Conditions are the presence/absence of an agent, value checks (=, >, <) on any IOP, etc.
- Advanced parameters consist in the control over an action's validity duration, reversibility or multiple triggers during a period of time and with a given periodicity.

![action_panel](/snapshots/action_panel.png)

Actions are usable either:

- in an actions panel for manual trigger by the user of the editor,
- or, in the timeline, in potentially very complex scenarios.

![actions](/snapshots/actions.png)

Support for actions and scenarios in the editor is still evolving quickly and will be enriched in the future.


## Platform data record and replay
A dedicated ingeScape agent gives record and replay capabilities to any ingScape system. When present, the editor is extended with record and replay control features. A system can be recorded at any time wether running a scenario or just executing on its own. 

Previous records are available for replay with several possibilities for the behavior of the agents towards recorded date. Previous records can also be exported for exploitation in third-party tools, for test campaigns or human factors experiments.





