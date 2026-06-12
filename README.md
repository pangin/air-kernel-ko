#EZ-FLASH  Air Kernel

### How to build 

    1.We use devkitARM_r53, you can use the current version or newer.
    2.Set the following environment variables in system, or modify the value in build.bat, based on your installation path
 
        PATH,DEVKITARM,DEVKITPRO,LIBGBA

    3.Double click on build.bat under winodws. If it goes well, you will get ezairkernel.gba
    4.Double click on  Link_kernel_image.exe link the ezairkernel.gba and image.bin to ezairkernel.bin, that is the Air kernel upgrade file