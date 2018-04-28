---
title: "Interactive environment for simulation and human factor experiments in aircraft cockpits"
date: 2018-04-25T11:56:11+02:00
draft: false
---
by [Ingenuity i/o](https://ingenuity.io)

This project was developed as a prototype to demonstrate new methods and techniques for digital collaborative interactions in aircraft cockpits based on multimodality and a large set of i/o devices (leap motion, tobii eye tracker, speech recognition, etc.).

{{< youtube 9Gr1Le_F7jU >}}

Started in 2014 and concluded in 2016, this project was at first developed with ad-hoc network solutions to insure the communication between the 7 different applications composing it. The applications are split between 5 distinct computers, respectively running Linux, macOS, Windows and iOS. Applications are developped in various technologies including Microsoft .Net (C#), Apple Cocoa (Objective-C), Qt (C++) and low-level C applications using various libraries. **This is the perfect definition of a complex heterogeneous system.**

The ingeScape platform enabled to port all the 7 applications into a unified ingeScape environment in three days only. The new version is completely supervised by the ingeScape editor and all communications are managed according to the ingeScape ***definition*** and ***mapping*** concepts.

The ingeScape library is embedded in all the applications. In order to switch to ingeScape, less than 5% of the total code had to be modified, thanks to the precise and simple use of the ingeScape library. Basically, previous network code has been completely disabled and replaced by ingeScape function calls, so that each agent can react to stimulated inputs to trigger its internal behaviors, and publish its outputs when relevant.

This project uses i/o devices with high output frequencies up to 120Hz, all using the same local network at the same time. The ingeScape platform introduces no noticeable latency in the system that previously relied on low level TCP and UDP data exchange.



