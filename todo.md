# Todo list

- Web client:
    - Make web client setup all asynchronous
    - Keep web client alive until intentionally closed
        - Figure out how to do this without multithreading concerns
- Web server:
    - Make detachable web server
    - Add initial JSON reading..?
    - Decision: Should the server decide what is correct, or just forward information?
        - Possible solution: Admin client has permission to rollback any change
            - Difficult but powerful
- Web:
    - All configuration should be in config files or arguments
        - Addresses, ports, etc

- General:
    - Logging is not threadsafe (or so I would assume). This needs fixing.
    - `abort_if` does the exact opposite of what it says it does.

- Bugs:
    - Critical:
        - Reloading config wipes the layout. (Is this fixed?)
    - Non-Critical:
        - Host leaving leads to playerlist being out of date, as with no host the next
            joining member (who becomes host) is not sent a playerlist.
        - 'ChatEdit' mode means we shouldn't parse keypress/keyrelease in GameContext
            - Probably 'BoardEdit' too? Wait, no

---

- Fixed:
    - Game freezes when clicking 'x' in text entry
    - Host leaving the game does not allow reassignment of the host.
