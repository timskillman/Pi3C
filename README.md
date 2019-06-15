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
  
[**3. Trying the Examples**](#tryexamples)
  - [Pi3C folders](#pi3cfolders)
  - [Projects Folder](#projects)

  - [Programming tools](#progtools)
  
[**4. Hello 3D World!**](#helloworld)

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
 - Open a terminal window enter;
 
 ~~~~
  sudo raspi-config
 ~~~~
 
 Select 'Advanced Options' and configure;
 
  - Full KMS, Open GL driver
  - 128 megabytes of GPU memory(memory split option)
  - Restart your Pi and setup SDL 2.0 (next section)
  
<a name="settingup">**Setting Up SDL 2.0 on your Raspberry Pi**</a>

Even though Raspbian Stretch comes with a version of SDL2, you need to download the latest SDL2 development libraries and SDL2's associated libraries for sound, images, network and font support.

Open a terminal window, make sure you're connected to the internet and enter (You will be prompted to enter 'Y' before installing each library);

~~~~
 sudo apt-get install libsdl2-dev
 sudo apt-get install libsdl2-mixer-dev
 sudo apt-get install libsdl2-ttf-dev
 sudo apt-get install libsdl2-image-dev
 sudo apt-get install libsdl2-net-dev
~~~~


<a name="downloadpi3c">**Downloading Pi3C**</a>

One of the easiest ways to download and modify Pi3C for your own use it use Git [(try this link if you want to know more)](https://projects.raspberrypi.org/en/projects/getting-started-with-git)

If Git is not already installed on your Pi, start by downloading Git; open a terminal window and type;

~~~~
 sudo apt install git
~~~~

Make sure you're in the 'home/pi' directory (indicated by the ~ (tilde character) with nothing after it) and enter;

~~~~
git clone https://github.com/timskillman/Pi3C.git
~~~~

Pi3C will be cloned onto your Pi and a Pi3C directory, with all the necessary code and resources, will be downloaded.

Now you should be setup and ready to play with Pi3C ... **Congratulations!**


  

<a name="tryexamples"></a>
# Trying the examples

<a name="pi3cfolders">**Pi3C folders**</a>

Pi3C folders are laid out in the following way;

~~~~
Projects
SharedCode
Resources
ThirdParty (Windows only)
~~~~

<a name="projects">**Projects Folder**</a>

The Projects folder contains all the examples to try out.  You can also add your own folder in this one or one next to it for creating your own projects.

So lets try out some examples!

Opening the Projects folder you will see several examples such as 'Castle Creator' and others.
Let's try out Castle Creator first and see how it works ...

Open a terminal window (if already closed) and navigate to;

~~~~
cd Pi3C/Projects/CastleCreator
~~~~

Now enter

~~~~
./CastleCreator
~~~~

*Note: If it doesn't work, you need to change CastleCreator's permissions and do the following ...*
*1. In the desktop folder, navigate to 'CastleCreator' and click the right button and select 'Permissions'*
*2. Under the Permissions tab, All three dropdowns should be set to 'Anyone' and then select OK.*
*3. Try ./CastleCreator again and it should run*

You will notice the 'options.txt' file and other txt files.  If you feel adventurous, try changing the screen resolution or set fullscreen to 'true'.  The 'options.txt' file is an easy of customising a game or an app without having to recompile your code.

Try the other examples such as Forest, Snow, 3D modeller and others using the same info for CastleCreator.


<a name="progtools">**Programming tools**</a>

