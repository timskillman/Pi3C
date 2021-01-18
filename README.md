![alt text](https://github.com/timskillman/Pi3C/blob/master/Resources/images/Pi3Clogo.png "Pi3C Logo")


# Pi3C
*v0.0.2 Alpha*

Pi3C is a fast, C++ graphics engine and toolkit tailored for all models of the Raspberry Pi and works on other platforms such as Windows. It's small size and innovative techniques make it fast enough to run highly detailed 3D games; even on the Pi Zero ... 

![alt text](https://github.com/timskillman/Pi3C/blob/master/Resources/images/ShipRear.png "Complex 3D models even on the Pi Zero")

Pi3C is designed to be simple to use and encourage youngsters (or anyone) new to programming by exploring the basic tutorials, games and apps provided.

These range from a few lines of simple, self explained code to more complex, real-world and heavy-weight examples.

Pi3C tries to avoid the complexities of C++ by providing a top-level class called Pi3C. This class has simple commands to help you start your journey into the exciting world of C++ programming.  Pi3C's utilizes the 'Simple DirectMedia Layer' (SDL 2.0) cross-platform development library (https://www.libsdl.org/). 

Pi3C provides functions to render Open GLES 2.0 graphics using shaders. It also provides a high speed, Immediate Mode Graphics User Interface (IMGUI) with various buttons, text  and common widgets. Using these functions, complex Apps and games can be made with relative ease.

Pi3C will also includes tools for 3D modelling and scene arrangemet with 1st person capabilites for testing scenes such as free flight, point and jump, and full screen renderng.  Other tools planned will include an IMGUI interface designer and C++ code generator that will jump-start your project.

Pi3C is currently under heavy development and functions may/will change over time as new Apps, Games and Demos are developed.  These developments will continue to greatly improve, enhance and simplify the Pi3C graphics engine and speed up your coding.

I hope that you will enjoy your experience with Pi3C as it develops!

*Tim Skillman*


# Contents

[**1. Introduction**](#introduction)

  - [What can I do with Pi3C?](#introduction)
  - [Example Games](#whatcanido)
  - [Example Apps](#examplegames)
  - [What is C++?](#exampleapps)

[**2. What you need and Setting up**](#whatyouneed)

  - [Raspberry Pi setup](#pisetup)
  - [Downloading Pi3C](#downloadpi3c)
  - [Setting up SDL 2.0 on the Raspberry Pi](#settingup)
  
[**3. Trying the Examples**](#tryexamples)
  - [Pi3C folders](#pi3cfolders)
  - [Projects Folder](#projects)
  - [Build, Run and Clean](#buildrunproj)
  - [Keeping up-to-date](#update)
  
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

The following games can be explored in this repo ...
- Invaders

<a name="exampleapps">**Example Apps**</a>

The following apps can be explored in this repo ...
- 3D Modeller
- Castle Creator
- Blocks (Minecraft experiment)

<a name="demos">**Example Demo's**</a>
- HelloWorld
- Snow
- Extruder
- PerlinMap
- ShaderToy
- MD viewer
- NoahsArk

<a name="whatisc++">**What is C++?**</a>

C++ is one of the most popular, industry standard programming languages.

----------------------------------------------

<a name="whatyouneed"></a>
# What you need and Setting up

<a name="pisetup">**Raspberry Pi setup**</a>

**You will need the following parts**

 - Raspberry Pi (preferably 3/4 for faster compiling)
 - 5 volt, 2.5 amp power supply with Micro-USB connector
 - Keyboard and Mouse
 - Monitor or TV with HMDI
 - HDMI cable
 - 16GB or 32GB Micro-SD card

<a name="settingup">**Setting up Pi3C**</a>

Pi3C requires SDL 2.0 and the fastest implementation of hardware Open GL.

1) Ensure the fastest Open GL driver is running

- Open a terminal window enter;
 
 ~~~~
  sudo raspi-config
 ~~~~
 
 Select 'Advanced Options' and configure;
 
  - Full KMS, Open GL driver
  - 64 megabytes of GPU memory(memory split option)
  - Restart your Pi and setup SDL 2.0 (next section)


2) <a name="downloadpi3c">**Downloading Pi3C**</a>

The easiest ways to download and modify Pi3C for your own use it use [Git](https://projects.raspberrypi.org/en/projects/getting-started-with-git)

Git is usually installed with the Raspberry Pi OS, but if not, download Git with;

~~~~
 sudo apt install git
~~~~

Make sure you're in the 'home/pi' directory (indicated by the ~ (tilde character) with nothing after it) and enter;

~~~~
git clone https://github.com/timskillman/Pi3C.git
~~~~

Pi3C will be cloned onto your Pi and a Pi3C directory, with all the necessary code and resources, will be downloaded.

3) <a name="settingup">**Setup your Raspberry Pi (All models including Pi4 and Zero)**</a>

Navigate to the Pi3C folder and enter:

~~~~
sudo sh Installer.sh
~~~~

This will install all the SDL 2.0 libraries, configure your GPU memory and enable the video drivers for your particular Raspberry Pi
When prompted, type "Reboot" to restart your Raspberry Pi.

Now you should be setup and ready to play with Pi3C ... **Congratulations!**


----------------------------------------------

<a name="tryexamples"></a>
# Trying the examples

<a name="pi3cfolders">**Pi3C folders**</a>

Pi3C folders are laid out in the following way;

~~~~
Examples     (Example games, apps and demos in here)
SharedCode   (Pi3C core and helper code)
Resources    (Models, textures and sounds stored here)
ThirdParty   (Windows only - not needed on the Pi, but keep if developing on a Windows machine as well)
Projects     A place to put your projects and reference the Pi3C SharedCode
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
 1. *In the desktop folder, navigate to 'CastleCreator' and click the right button and select 'Permissions'*
 2. *Under the Permissions tab, All three dropdowns should be set to 'Anyone' and then select OK.*
 3. *Try ./CastleCreator again and it should run*

You will notice the '**options.txt**' file and other txt files in the CastleCreator folder.  If you feel adventurous, open these files and try changing the screen resolution or set fullscreen to 'true'.  The 'options.txt' file is an easy way of customising a game or an app without having to recompile your code.

Try the other examples such as **Invaders, Snow, 3D modeller** and others using the same info for CastleCreator.

Of course, we're trying out precompiled examples - so let's get coding your own stuff!

<a name="buildrunproj"></a>
## Build, Run and Clean

If setup correctly, Pi3C examples and tools should be ready to build and run.  
To build a project do the following;

**1. Enter the Project folder you want to build**
~~~~
cd Invaders
~~~~
Note: you don't need to type the whole word 'Invaders' ... type the first few characters such as 'Inv' and then press the TAB key ... this will auto-complete the word. You can do this shortcut for any part of a long pathname for example.

**2. Build with 'make'**
~~~~
make
~~~~

Make will build the whole project and all of its dependent code.  

Dependent code in this case is 'SharedCode' containing Pi3C.  If not built previously, Pi3C code will also build and it can take some time.  However, this only happens once since the compiler is clever enough to only compile code that has changed after that.

Once built, you will notice a "_build" folder appearing next to the Pi3C folder.  This folder contains all the object files created from the source.  These files are then 'linked' together to create the final program.

**3. Run**

You will also notice the built, executable file in your project folder.  
To run the executable on the Raspberry Pi (Invaders for example), type;

~~~~
./Invaders
~~~~

**4. Clean**

Sometimes your project won't compile as expected - and that's not due to any bad coding! ... For example, when you update Pi3C the object files in the build folder need cleaning out.  To clean the project entirely, type;

~~~~
make clean
~~~~

This basically deletes all the object files in the build folder (note: don't worrry, it won't delete anything important)
Cleaning will ensure that all your built code is fresh and using the latest changes.

You should clean after updating Pi3C or making significant changes.  However, it is not necessary if Pi3C SharedCode has not changed. 

<a name="update"></a>
## Keep up-to-date

Keeping up-to-date with the latest code is simple.  In a terminal window navigate to the Pi3C folder,  and type;

~~~~
git pull origin master
~~~~

This command will import (pull) all the latest Pi3C changes from the GIT repository.



