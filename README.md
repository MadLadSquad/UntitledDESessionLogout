# UntitledDESessionLogout
The session logout utility for the UntitledDesktopEnvironment. It provides both a simple CLI and GUI method to log out of the current session. Here are the
currently supported features:
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
