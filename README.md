# quadtree-compression
This repository contains the result of a coding assignment completed for UBC's CPSC 221 course.

Despite its origin as homework, I have decided to upload it, since it allows for a lot of insight into my current coding skills. I think the code shows very well, that:
- I can work on a large-scale project by myself 
- I can break the high-level goal up into many small steps
- My code is legible and well-commented (very important to me)

The original assignment required us to write the code for a quadtree compression algorithm as described [here](https://www.ugrad.cs.ubc.ca/~cs221/2018W2/mps/p3/). Provided with the algorithm's logic, images of its desired outcome, and a bare-bone skeleton code, I put myself to work.

As suggested by the instructions, I first had to come up with a clever method to compute the _statistics_ necessary for the compilation of a so-called _toqutree_ (torus-quadtree). In a second step, I had to apply the statistics in order to convert an input image recursively into a quadtree structure. Accordingly, my main contribution can be found in the following files:
- `stats.h` and `stats.cpp`
- `toqutree.h` and `toqutree.cpp`

In order to validate my code, I've written numerous test-cases of increasing fidelity. They can be found (partly commented-out) in `testComp.cpp`.

The resulting compression met all the course's requirement and got the full score. As an example, we were provided with an image that had been rendered by the instructor's algorithm. The comparison to my result shows very little deviation, that can only be explained by rounding errors and differences in the order of 2D-traversals of pixels.
