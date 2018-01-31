# Introduction
CppWeb is a C++ starter project for developing a web service common to Microservice Architectures.  It is built upon the excellent example source code from the Boost Beast library but packaged and extended in a manner that is more similar to the style of web service development that I've grown familiar to in the Node.js, Python, C# world.

I created this to share for anyone else interested in bringing C++ to the web.  Admittedly, this is my first piece of work after a long hiatus from developing in C++ so there is certainly room for improvement.  Please be sure to use this as a template to learn from and improve upon and if you find bugs or have suggestions, please help fix and contribute! 

C++ was one of the first languages that I developed in and in comparison to some other languages can feel at times much more difficult to setup and work in.  However, I believe that it has the potential to do much more with less overhead and just needs a more familiar interface for people to build upon.  I hope for this project to expose more developers in this very cloud-centric web-focused world to C++ and to also provide a path for experienced C++ developers to have a simple framework to build a web service upon.

# Applications and Exercises

One thing that you could immediately do with this project, which I have done for my own work, is create a route handler that creates a client that then retrieves information from another API, and then returns the response back through this service.

# Dependencies

* Boost with Beast (1.66.0)
* OpenSSL

# Building

This project includes a Makefile that is used by the Dockerfile at the root of the project.  The Makefile can be further modified very slightly to allow building natively on a Mac with libraries installed through homebrew.

To build this project natively, please inspect the Dockerfile for how it fetches dependencies and builds them.  The steps in the Dockerfile are coupled with the provided Makefile.  

# Running
There are currently two environment variables required for running the basic project.

* PORT: The port number to listen on.
* THREADS: The number of threads to give to the program.

A script file, docker.sh, is provided that can be used to build and run a docker image containing the source code.

# Contributing
This is a very basic starter project but it does give you an interface to easily extend to write your own route handlers.  I have lots of ideas for furthering this project so that it could handle common use cases that come up with writing web services.

* An OAuth2 client that handles authentication and token management to other APIs.  
* Some middleware framework to incorporate a database framework.
* Adding some sort of authentication module to this framework so that callers could be authorized.
* A real root certificate store implementation that fetches from the system.

# License
MIT License
