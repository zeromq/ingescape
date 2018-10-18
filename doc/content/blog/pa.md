+++
title = "Public Announce monitoring system"
date = "2018-04-20T11:56:11+02:00"
categories = ["works"]
banner = "/img/banners/pa.png"
+++
by [Ingenuity i/o](https://ingenuity.io)

This operational project resulted in an ecosystem of applications dedicated to scheduling, monitoring and operating Public Announce systems in all kinds of places (malls, train stations, schools, public places, etc.). Each site is equipped with various types of devices from speakers to command and monitoring boxes. 

![PA application](/images/pa.png)

ingeScape was used to create two types of software agents :

- *software proxies* enabling the communication with the electronic equipments in the different areas of a given site, relying on heterogenous and specific protocols (SIP, web services REST/JSON, RS232 encapsulated in TCP/IP) and managing the broadcast schedules,
- *graphical applications* used by different profiles of operators (hostesses, supervisors and administrators).

The software proxies run on appliances based on Linux. The graphical applications run on Microsoft Windows (7, 8 and 10).

Redundancy between proxies is in place, based on ingeScape discovery mechanisms. ingeScape insures the synchronization between the proxies and the various graphical applications that are used simultaneously and sometimes concurrently between operators.

The applications make use of a wide variety of IOP types, namely strings for text messages, raw data to exchange media files and media streams and numeric types for well-identified values (on/off commands, sound volume, presence monitoring, etc.).
