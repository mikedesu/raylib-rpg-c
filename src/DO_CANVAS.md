Take a look at the file `minshell.html`.
I have gotten this game to build and run in the web browser and it loads and works on itch.io.
However, some users are reporting various screen dimension and position issues across different browsers and operating systems, even though it works for me.
The game window and canvas are set to 1920x1080, but some users with the same or higher OS dimensions report the game's display appearing off-center and being cut-off in the browser.
I thought maybe we could take a look at the HTML template used to run the game and examine the canvas to see if there is anything we can do to help force the canvas to conform to the window.
