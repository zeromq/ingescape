+++
title = "Operational supervision environment for RER Ligne A in Paris"
date = "2018-04-20T11:57:11+02:00"
categories = ["works"]
banner = "img/banners/ratp.png"
+++
by [Ingenuity i/o](https://ingenuity.io)

This is a "real world" operational project involving active communication with critical rail systems. It introduces innovative paperless multitouch interactive workstations enabling to command and control operational equipments and to monitor all the human and train resources for the exploitation of the line. It can be seen as **a combination of a large IoT network and a Big Data system** with lots of data sources to be merged in order to insure proper situation awareness and control means for the operators.

![innovative workstation](/images/pgim.png)

Each workstation is composed of 5 applications running under Linux, 3 of them developed in C and the 2 others forming the graphical environment, in Qt/C++.

Massive communication flows are handled by the applications to:

- display real-time state of all the field equipments (signals, branches, etc.), 
- relay user commands to the field,
- locate all the trains in real-time,
- feed all the user interfaces with results from the computation algorithms (schedules, resources allocation, alerts and notifications, etc.).

This project has widely benefited from the iterative architecture and integration capabilities supported by ingeScape. Tests around connections and data exchange have started very early in the global process, enabling to quickly provide proof of concepts and to create a positive loop between engineering and UX design activites, one providing feedback to the other about opportunities and limits.

Logging and record capabilities as offered by ingeScape have been actively included in the project activities for archiving, training and technical validation.