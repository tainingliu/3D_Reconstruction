3D_Reconstruction
=================

4th Year Design project at UofT that generates 3D coordinates from pictures of rectangular prisms.

Was made in a period of 10 months for the capstone project in my computer engineering undergrad. It is meant to be able to take in any image of a rectangular prism and through a series of transforms, generate the 3D coordinates of the object relative to the camera. It was tested using images generated using an open source 3D modeling tool called blender. Unfortunately, it proved extremely difficult to try and generate coordinates from real life images as the settings for the edge detection proved difficult to control.

It uses OpenCV for the image processing, acquring edges and straightlines, and implements my own method for generating the 3D coordinates from that data.

As the project stands now, May 14, 2013, it is able to take in simple generated models from Blender and generate the 3D coordinates from them. The code itself is a good representation of my programming skill at the time ( no matter how poor that may be ) and will hopefully be updated in the future.

Any questions can be directed to my gmail at taining.liu@gmail.com.
