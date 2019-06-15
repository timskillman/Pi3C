![alt text](https://github.com/timskillman/Pi3C/blob/master/Resources/images/Pi3Clogo.png "Pi3C Logo")


# Pi3C

Pi3C is a C++ graphics engine and toolkit designed for the Raspberry Pi and works on other platforms such as Windows and Android.

It is designed to encourage youngsters and anyone interested in programming by exploring the games and apps provided.

These range from a few lines of simple, self explained code to more complex, real-world examples.

Pi3C tries to avoid the complexities of C++ by providing a top-level class called Pi3C. This class has simple commands to help you start your journey into the exciting world of programming.

Even though Pi3C is simple, is provides top class functions to render incredible graphics when you consider the low cost of a Raspberry Pi.
In fact, I'm confident that you will be amazed at what can be achieved on the Raspberry with just a few lines of code using Pi3C.

Pi3C also includes tools such as a 3D modeller and games editor with 1st person capabilites for testing scenes.  
Other tools will include a Style editor (to help design a Graphics User Interface (GUI)) and a project kickstarter.

I hope that you will enjoy your experience with Pi3C!

*Tim Skillman*


# Contents

[**1. Introduction**](#introduction)

  - [What can I do with Pi3C?](#introduction)
  - [Example Games](#whatcanido)
  - [Example Apps](#examplegames)
  - [What is C++?](#exampleapps)

[**2. What you need and Setting up**](#whatyouneed)

  - [Raspberry Pi setup](#pisetup)
  - [Setting up SDL 2.0 on the Raspberry Pi](#settingup)
  - [Downloading Pi3C](#downloadpi3c)
  - [Programming tools](#progtools)
  
[**3. Hello 3D World!**](#helloworld)

  - The main program
  - Including stuff
  - Compile & Run


----------------------------------------------

<a name="introduction"></a>
# Introduction

<a name="whatcanido">**What can I do with Pi3C?**</a>

With Pi3C you can write high speed 2D or 3D games.
Or, you can write fast applications.

<a name="examplegames">**Example Games**</a>

The following examples can be explored in this repo ...

<a name="exampleapps">**Example Apps**</a>

The following apps can be explored in this repo ...

<a name="whatisc++">**What is C++?**</a>

C++ is one of the most popular, industry standard programming languages.

----------------------------------------------

<a name="whatyouneed"></a>
# What you need and Setting up

<a name="pisetup">**Raspberry Pi setup**</a>

**You will need the following parts**

 - Raspberry Pi Model 3 B+
 - 5 volt, 2.5 amp power supply with Micro-USB connector
 - Keyboard and Mouse
 - Monitor or TV with HMDI
 - HDMI cable
 - 16GB or 32GB Micro-SD card

**Setting up the SD card**

 - Download the latest Raspbian Stretch image from [https://www.raspberrypi.org/downloads/](https://www.raspberrypi.org/downloads/)
 - Flash the Raspbian Stretch image to your SD card using [balenaEtcher](https://www.balena.io/etcher/)
 - Insert the SD card into your Raspberry Pi, [setup the Pi](https://projects.raspberrypi.org/en/projects/raspberry-pi-setting-up) and start it up.  You will need WiFi setup as well.
 - Open a task window enter;
 
 ~~~~
 $ sudo raspi-config
 ~~~~
 
 Select 'Advanced Options' and configure;
 
  - Full OpenGL ES driver
  - 128 megabytes of GPU memory
  - Restart your Pi and setup SDL 2.0 (next section)
  
<a name="settingup">**Setting Up SDL 2.0 on your Raspberry Pi**</a>

Even though Raspbian Stretch comes with a version of SDL2, you need to download the latest SDL2 development libraries and SDL2's associated libraries for sound, images, network and font support.

Open a task window, make sure you're connected to the internet and enter;

~~~~
$ sudo apt-get install libsdl2-dev
$ sudo apt-get install libsdl2-mixer-dev
$ sudo apt-get install libsdl2-ttf-dev
$ sudo apt-get install libsdl2-image-dev
$ sudo apt-get install libsdl2-net-dev
~~~~


<a name="downloadpi3c">**Downloading Pi3C**</a>

One of the easiest ways to download and modify Pi3C for you own use it use Git [(try this link if you want to know more)](https://projects.raspberrypi.org/en/projects/getting-started-with-git)

Start by downloading Git to your Raspberry Pi ... open a task window and type;

~~~~
$ sudo apt install git
~~~~



<a name="progtools">**Programming tools**</a>

