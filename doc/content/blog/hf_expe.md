+++
title = "Study of new interaction modalities for pilot tasks carried out in a simulator"
date = "2018-04-20T11:55:11+02:00"
categories = ["works"]
banner = "img/banners/hf_expe.jpg"
+++
by [ESTIA](http://www.estia.fr/)
 
The **multidisciplinary team** from the PEPSS platform  was interested in comparing 3 interaction modalities to achieve the task of controlling the engine power of an aircraft. We wanted to explore new digital interactions for this specific task. Three interactions were compared: the classical one using a **physical throttle**, an innovative one using a **touch screen** and a disruptive one using the *grab* specific gesture on a **leap motion** device. The flight simulation was provided by the open-source software **Flight Gear**.

![setup](/images/hf_expe.jpg)

The classic approach would have been to implement a hardcoded software prototype for our specific scenarios.  But this kind of project needs to be carried out in an *agile*, *iterative* and *multidisciplinary* context and the system need to be flexible enough to easily integrate the new use cases, design modifications and experimentation protocols, including at runtime. And in addition, the experimenters were ergonomic students without technical knowledge and development abilities.
 
As our developer team already uses **ingeScape** to develop  interaction agents, including an UDP bridge to interact with Flight Gear, it was really easy to set-up the experimentation by reusing these exiting ingeScape agents, literally with **no cost-time development**. The students were autonomous in the solution design and experimentation control at runtime, using the ingeScape editor. That is a great example of ingeScape enabling the reuse agents for a totally different usage than the initial one, thus saving development time. And also modifying and adjusting the behavior of a system in real time without any technical knowledge, thus giving the control to the experimenters.