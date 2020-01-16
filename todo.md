# Todo list

- Rewrite all web code to have a better structure.
- Improve display layout, add resizing support.
- Add GIF to Readme (perhaps with streamlined installation instructions)
- Add support for 'bonus' configuration (e.g. using 5 tiles in a turn gives +10)
- Add manual points assignment command
    - Complete, not tested.
- Add 'help' commands for each command.

- Web client:
- Web server:
    - Make detachable web server
        - Partially complete. Application can be launched in server mode, which never opens a GUI.
    - Add initial JSON reading..?
        - Not sure what this referred to. Probably automatic configuration from a file, which is low priority.
    - Decision: Should the server decide what is correct, or just forward information?
        - Possible solution: Admin client has permission to rollback any change
            - Difficult but powerful
        - Current solution:
            - Server knows almost nothing, and only forwards information between clients.
            - Several issues with this, but it does keep web code from being entangled with game code.
- Web:
    - All configuration should be in config files or arguments
        - Addresses, ports, etc

- General:
    - Logging is not threadsafe (or so I would assume). This needs fixing.

- Bugs:
    - Critical:
        - Scoring space causes... issues
            - Need way to delete tile
    - Non-Critical:
        - Host leaving leads to playerlist being out of date, as with no host the next
            joining member (who becomes host) is not sent a playerlist.
        - 'ChatEdit' mode means we shouldn't parse keypress/keyrelease in GameContext
            - Probably 'BoardEdit' too? Wait, no
        - Reloading config wipes the layout. (Is this fixed?)
            - Moved to non-critical, as I don't think this is a problem currently.

---

- Fixed:
    - Game freezes when clicking 'x' in text entry
    - Host leaving the game does not allow reassignment of the host.
    - Entering a wildcard doesn't work most of the time for whatever reason
        - Fixed by allowing game context to take control of keyboard fully for tile input.

- Complete:
    - Web client:
        - Make web client setup all asynchronous
            - Completed early on.
        - Keep web client alive until intentionally closed
            - Figure out how to do this without multithreading concerns
                - Completed, web client runs in its own thread.
    - Web server:
