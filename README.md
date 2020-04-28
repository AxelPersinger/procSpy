# procSpy

A friend of mine asked me if there was a way to spy on any arbitrary process' STDIN as they were created. They didn't find any answers online at the time of writing this so I decided to write one up myself.

Goals:

* Automatically capture processes as they start
* Non-intrusive to processes (processes shouldn't know they're being monitored)
* Monitor STDIN, STDOUT, STDERR
* Write codebase in as vanilla Linux as possible

