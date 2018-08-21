+++
title = "Voice-based weather service"
date = "2018-04-23T11:36:41+02:00"
categories = ["agents"]
banner = "img/banners/your_first_agent.png"
+++

ingeScape offers slick integration for heterogeneous software agents and make them interoperable to provide higher level, high value services to end-users. This article shows a typical example by creating a voice-based weather service, capable of: 

- Receiving voice queries identifiying specific areas, 
- Interrogating a web service to get weather information for these areas,
- And using speech synthesis to communicate the received weather information.



### Overall architecture

Here is the platform architecture as seen in the ingeScape editor:

[PHOTO]

This example platform uses three different software agents running on three different software platforms (Microsoft Windows, Linux CentOS 7 and Apple macOS) and using an external remote web service to get weather information that is of course not shown in the editor.

### Voice recognition
The **GeoVoiceRecognition** agent is a basic software agent developed on Microsoft Windows, using the Microsoft Voice Recognition module in Microsoft .Net. This is the simplest choice to introduce voice recognition on Windows. It could be replaced by any other solution that fits your actual needs.

Our **GeoVoiceRecognition** agent has been designed to react to queries like "What is the weather in Toulouse ?", "What is the weather for zip code 31520 ?" and to provide the recognized city name or zip code as an output.

This agent is composed of the following IOPs:

- inputs
	- *none*
- outputs
	- *city* : a string providing the recognized city name
	- *zipCode* : a string providing the recognized zip code
- parameters
	- *country* : a string representing the current country using the ISO 3166 country codes
	
We decided to keep this agent extremely simple to fit our exact needs. It would be easy to enrich the voice recognition capabilities and to use more advanced features to capture GPS coordinates, circle or rectangle geographic areas, etc. Speech recognition specialists would achieve that very quickly and could create additional outputs to our **GeoVoiceRecognition** agent.

Note that we could also develop a fully generic **VoiceRecognition** agent exposing a large set of parameters and relying on major voice recognition features and models such as speech grammars. Such an agent could react on other key words, could handle natural speech recognition, would be able to achieve sophisticated semantic extraction, etc. 

This is one cool thing with ingeScape : you can make your agents evolve in time without breaking things that already work and without having any impact on the other agents of your platforms. Even a radical change in an internal implementation can be completely invisible to other people using your agent based on its definition.


### Weather information
The choice of having our **GeoVoiceRecognition** agent provide city names and zip codes is justified by the requirements of the external web service we are using to get weather information. We are using the [OpenWeatherMap](https://openweathermap.org/) web [API](https://openweathermap.org/current), which handles various types of queries, including ones providing city names and zip codes.

We decided to have our **GeoVoiceRecognition** agent take the responsibility of identifying the type of query (city or zip code) because semantic interpretations are usually part of the *voice recognition* or *natural language processing* domains of expertise.

Our **OpenWeatherMap** agent is composed of the following IOPs:

- inputs
	- *city*: to send specific queries related to a city name
	- *zipCode*: another specific query type using zip codes, which is very similar to the city type.
- outputs
	- *forecast* : a string providing weather forecast based on a city or zip code query, that can be read by a human or uttered by a speech synthesis engine.
- parameters
	- *apiKey* : the API key to use the web service

The [OpenWeatherMap](https://openweathermap.org/) service returns information structured in JSON texts. Our **OpenWeatherMap** agent has the responsibility to transform the JSON data into a nice human-readable text. Once again, such a transformation seems to fit with our **OpenWeatherMap** agent purpose and it would make less sense achieving this transformation elsewhere : the JSON data format is specific to each weather service we could use and its interpretation is part of exploiting the service.

In this example platform, the **OpenWeatherMap** agent is the only one requiring an access to the internet. Because it runs as an independent software, it is easy and straightforward to manage its internet access and attached security constraints. All the other agents inside your local network benefit from the service without any additional network configuration.


### Text to speech
Our **OpenWeatherMap** agent provides nice forecast texts that can be injected directly into a text-to-speech service. To implement such a service into a text-to-speech agent we decided to create a Cocoa application on a Mac mini that is conveniently connected to speakers in our offices.

Text-to-speech is simple enough to implement, so that we can develop a generic agent that is capable of covering most of the  features we need. Of course, some of our agent parameters depend on the peculiarities of the choices made by Apple in their Cocoa NSSpeechSynthesizer class but similar parameters can be expected in all other test-to-speech libraries. A ton of parameters about voice configuration are available. For now, we will not expose them in our IOPs : we can do that later if needed.

Our **macTextToSpeech** agent is composed of the following IOPs:

- inputs
	- *speak* : a string input providing text to be uttered
	- *recordIn* : a string input providing text to be generated as sound data on the dedicated output
- outputs
	- *recordOut* : a data output providing a sound stream formatted as a Waveform at 22kHz, 16bit, mono
- parameters
	- *keepRecords* : a boolean flag to decide if we keep the generated wav records or not (default is false)
	- *recordsPath* : path to save the records to (default is the macOS temp folder for the current user)

The two parameters are a sort of bonus for platform administrators : it enables them to decide if and where to store generated sound files, either to archive them, or to store them in a path that provides enough disk space.

NB: In the situation where you want to generate both sound data and utterance on the speakers, you just need to send the same string on both *speak* and *recordIn* inputs. The agent will handle them in parallel.
