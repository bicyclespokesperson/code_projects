## Math library

I probably want to use the gltf [crate](https://docs.rs/gltf/latest/gltf/). I also need to figure out how much of the viewer I can/want to implement, and how much is done for me. Then I can use the linear algebra types 
used there to work on the actual simulation.

This [viewer](https://github.com/bwasty/gltf-viewer) might be good? 

Options
  Add the disc flight path to the gltf model
  Make a gltf model of the disc path separately, then render both models
  Render the disc flight path in the viewer, separately from the model

The gltf crate implements its own 3d and 4d vector types. They're pretty simple, I don't even see inner/outer products.

The other option is nalgebra, or nalgebra-glm. 

Considerations
  Want to support animation eventually?


## Spin stabilized sim notes

https://aviation.stackexchange.com/questions/83993/the-relation-between-euler-angle-rate-and-body-axis-rates

## Original reddit post

https://www.reddit.com/r/discgolf/comments/kflnrm/i_made_a_physics_based_throw_simulator/
https://colab.research.google.com/drive/1fwoivf9S6tro1A23yEetisPgexmhfjIK?usp=sharing#scrollTo=1rCpM6LuUZcV


