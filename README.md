# UntitledDESessionLogout
[![CI](https://github.com/MadLadSquad/UntitledDESessionLogout/actions/workflows/CI.yaml/badge.svg)](https://github.com/MadLadSquad/UntitledDESessionLogout/actions/workflows/CI.yaml)
[![MIT license](https://img.shields.io/badge/License-MIT-blue.svg)](https://lbesson.mit-license.org/)
[![Discord](https://img.shields.io/discord/717037253292982315.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/4wgH8ZE)

![image](https://github.com/MadLadSquad/UntitledDESessionLogout/assets/40400590/ac666164-9f37-49ce-802c-353556b0d3bf)


The session logout utility for the UntitledDesktopEnvironment. It provides both a simple CLI and GUI method to log out of the 
current session. Currently supported features:
- [x] Log out the user from the current session
- [x] Restart the user's computer
- [x] Power off the user's computer
- [x] Suspend the user
- [x] Hybernate the user

## Preview
![image](https://user-images.githubusercontent.com/40400590/208991632-61d382d0-9bcd-435a-9c23-45017f0904c5.png)


## Dependencies
This application depends on the following outside the standard UntitledImGuiFramework:
1. dbus
1. elogind
1. polkit

Additionally the user needs to be a part of the `power` group to be part of this, to create the group simply run the following:
1. `root # groupadd power`
1. `root # gpasswd -a <username> power`

## Building
This is a standard [UntitledImGuiFramework](https://github.com/MadLadSquad/UntitledImGuiFramework) project. Building instructions can be found 
[here](https://github.com/MadLadSquad/UntitledImGuiFramework/wiki/Collaborating-on-a-project-with-the-framework#setting-up-another-persons-project).
