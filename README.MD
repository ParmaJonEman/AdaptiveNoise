### freqfilter
### Author: Jon Eman
### Date: 3/7/2023

### Build
##### To build, install OpenCV on your computer, change the OpenCV path in the CMAKE file, and use the standard CMAKE commands for building.

### Usage

##### Run the program with the following command structure:
    adaptive [-h] [-n=N] <imagefile1> <imagefile2> <imagefile3>

##### imageFile1 is the image you wish you filter. imageFile2 is the filename you wish to save the adaptive mean filtered image.
##### imageFile3 is the filename you wish to save the adaptive median filtered image.
##### -n is the option for how much noise to add to the image. Default is 10%.
