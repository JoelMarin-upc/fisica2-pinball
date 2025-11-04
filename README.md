# Pinball

## Description

This project replicates a famous game named Pinball, that consist on flipping leavers in order for the ball to not fall from the board.
The project uses the Box2D v2.4.1 library to simulate realistic physics, and it also uses the raylib v5.5.0 library to render graphics on the screen.

## Key Features

 - You have 3 balls at start
 - Balls launched from starting position
 - Activate flippers to keep the ball in the board
 - Points are added for hitting objects
 - If the ball falls off the board you can throw the next ball 
 - If there are no more balls you lose and the game restarts 
 
## Controls

 ### Game controls
 - **LEFT ARROW**: Left flipper
 - **RIGHT ARROW**: Right flipper
 - **DOWN ARROW**: Bumper
 
 ### Debug controls
 - **F1**: Enables/Disables debug mode (Draws colliders, locks ball to mouse)
 - **C**: Enables/Disables physics
 - **X**: Increases gravity by 1 m/s^2
 - **Z**: Decreases gravity by 1 m/s^2
 - **M**: Increases restitution coefficient by 0.1
 - **N**: Decreases restitution coefficient by 0.1
 - **TAB**: Automatically triggers bonus

## Developers

 - Biel Cadenas Benedicto
 - Pau Gallego Guerrero
 - David Alejandro García Castro
 - Joel Marin Lluset

## License

This project is licensed under an unmodified MIT license, which is an OSI-certified license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
