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
