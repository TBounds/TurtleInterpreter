Turtle Graphics Interpreter
Name: Tyler Bounds
Email: tyler.bounds@wsu.edu

Modification History:
  Initial files created.... W. Cochran, wcochran@wsu.edu (2/15/2026)

Description:
  A simple interpreter that reads in a "Turtle Graphics" program,
  inteprets the program, which in turn outputs a sequence
  of commands which control a "virtual turtle" which can draw
  interesting pictures on the 2D plane with a pen sticking
  out of its belly.

Turtle Graphics Language:
  See the associated project documentation for more details.
  Here is an example Turtle program for generating a spiral.

    N := 20         # number of turns in spiral
    ANGLE := 45     # turning angle
    DIST := 10      # step side
    SCALE := 0.85   # amount to scale step size with
    I := 1
    WHILE I <= N DO
      FORWARD DIST
      LEFT ANGLE
      DIST := DIST * SCALE
      I := I + 1
    OD

Building the Interpretter:
  A Makefile has been provided for building the "turtle" interpretter.
  
    make scannertest   # build scannertest using test program
    make turtle        # build turtle interpretter "turtle"

Output of the interpretter:
  
    The interpreter takes Turtle source code an generates a text file
    listing a sequence of turtle "commands."
       Command       Description
       ---------     ------------------------------------------------
        H             Transports turtle "home;" i.e., the origin (0,0)
        U             Pen Up
        D             Pen Down
        M <dist>      Move forward <dist> units (draws line if pen down)
        R <angle>     Rotate <angle> degrees
        [             Push turtle state (position, direction, pen up/down)
        ]             Pop turtle state (restore state to last push)

Generating images:
  The "turtle.pl" Perl script converts turtle commands into PGM images:

    ./turtle spiral.turtle | ./turtle.pl > spiral.pgm

  To convert PGM images into other formats, install/usr the 
  ImageMagick "convert" command (see http://imagemagick.org/).
  For example, the following command runs the "turtle" intepreter
  and pipes the output to the "turtle.pl" script which sends
  a PGM image to the "convert" utility which creates a PNG image:

    ./turtle spiral.turtle | ./turtle.pl | convert - turtle.png


Contents of Archive:

README.txt ........ This file.
Scanner.{h,cpp} ... Lexical Analyzer.
scannertest.cpp ... Lexical Anayzer unit test.
AST.h ............. Abstract Syntax Trees (AST) for expressions/statements.
Env.{h,cpp} ....... Environment / Symbol Table.
Parser.{h,cpp} .... Syntax Analyzer.
turtle.cpp ........ Interpretter main.
turtle.pl ......... Scripts that sraw PGM image from turtle commands.
Makefile .......... Builds "turtle" interpretter 
scantest.pl ....... Runs "scannertest" on examples/*.turtle
parsetest.pl ...... Runs "turtle" on examples/*.turtle
makeimages.pl ..... Creates images from examples/*.turtle
examples/ ......... Example turtle programs.
tokens/ ........... Output of "scannertest" to compare with in scantest.pl
commands/ ......... Output of "turtle" to compare with in parsetest.pl
images/ ........... Output images