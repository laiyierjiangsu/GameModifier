GameModifier
==========
A simple game editor which can edit game by modify memory、inject dll。
Hook
Work: debughook、Remoteinject
Bad: apc、thread、process、
Need test: ime, reg


InjectedDll
==========
Generate a dll which will be used to be injected to other process.

GameGuard
==========
A project which will be used to detect dll injection、debugging and modification.

BlackBone
==========
https://github.com/DarthTon/Blackbone.git
An excellent project used for Windows memory hacking library

Tamper Aware and Self Healing Code
=====================
Detect modification
https://www.codeproject.com/Articles/18961/Tamper-Aware-and-Self-Healing-Code

Todo list
==========
1、How to detect writeprocessmemory?
2、PE file check doesn't work?
3、Clear the debug port?
4、Get the callstack of parent function?
5、Hardware breakpoint and software breakpoint?
6、How to enumerate all windows of current desktop?
7、How to find the base addr of variable?

